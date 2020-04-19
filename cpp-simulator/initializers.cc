//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

#include "models.h"
#include "initializers.h"

#ifdef DEBUG
#include <cassert>
#endif

using std::string;
using std::vector;
using std::to_string;

auto readJSONFile(string filename){
  std::ifstream ifs(filename, std::ifstream::in);
  rapidjson::IStreamWrapper isw(ifs);
  rapidjson::Document d;
  d.ParseStream(isw);
  return d;
}

auto prettyJSON(const rapidjson::Document& d){
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  d.Accept(writer);
  return buffer.GetString();
}


vector<house> init_homes(){
  auto houseJSON = readJSONFile(GLOBAL.input_base + "houses.json");
  auto size = houseJSON.GetArray().Size();
  vector<house> homes(size);
  GLOBAL.num_homes = size;

  count_type index = 0;
  for (auto &elem: houseJSON.GetArray()){
	homes[index].set(elem["lat"].GetDouble(),
					 elem["lon"].GetDouble(),
					 compliance());
	++index;
  }
  return homes;
}

vector<workplace> init_workplaces() {
  auto schoolJSON = readJSONFile(GLOBAL.input_base + "schools.json");
  auto wpJSON = readJSONFile(GLOBAL.input_base + "workplaces.json");

  auto school_size = schoolJSON.GetArray().Size();
  GLOBAL.num_schools = school_size;

  auto wp_size = wpJSON.GetArray().Size();
  GLOBAL.num_workplaces = wp_size;

  auto size = wp_size +  school_size;
  vector<workplace> wps(size);

  count_type index = 0;
  // schools come first followed by workspaces, as in the JSON version
  for (auto &elem: schoolJSON.GetArray()){
	wps[index].set(elem["lat"].GetDouble(),
				   elem["lon"].GetDouble(),
				   WorkplaceType::school);
	++index;
  }
  assert(index == GLOBAL.num_schools);
  for (auto &elem: wpJSON.GetArray()){
	wps[index].set(elem["lat"].GetDouble(),
				   elem["lon"].GetDouble(),
				   WorkplaceType::office);
	++index;
  }
  assert(index == GLOBAL.num_schools + GLOBAL.num_workplaces);
  return wps;
}

vector<community> init_community() {
  auto comJSON = readJSONFile(GLOBAL.input_base + "commonArea.json");

  auto size = comJSON.GetArray().Size();
  GLOBAL.num_communities = size;

  vector<community> communities(size);

  count_type index = 0;
  
  for (auto &elem: comJSON.GetArray()){
	communities[index].set(elem["lat"].GetDouble(),
						   elem["lon"].GetDouble(),
						   elem["wardNo"].GetInt());
	++index;
  }
  assert(index == GLOBAL.num_communities);
  sort(communities.begin(), communities.end(),
	   [](const auto& a, const auto& b){
		 return a.wardNo < b.wardNo;
	   });
  return communities;
}


vector<double> compute_prob_infection_given_community(double infection_probability, bool set_uniform){
  auto fracPopJSON = readJSONFile(GLOBAL.input_base + "fractionPopulation.json");
  auto num_communities = fracPopJSON.GetArray().Size();
  if(set_uniform){
	return vector<double>(num_communities, infection_probability);
  }
  else {
	auto fracQuarantinesJSON = readJSONFile(GLOBAL.input_base + "quarantinedPopulation.json");
	const rapidjson::Value& quar_array = fracQuarantinesJSON.GetArray();
	const rapidjson::Value& frac_array = fracPopJSON.GetArray();
	vector<double> prob_infec_given_community(num_communities);
	for(count_type index = 0; index < num_communities; ++index){
	  prob_infec_given_community[index] =
		infection_probability
		* quar_array[index]["fracQuarantined"].GetDouble()
		/ frac_array[index]["fracPopulation"].GetDouble();
	}
	return prob_infec_given_community;
  }
}

void seed_initial_infection_at_node(agent& node, double time_of_infection){
  node.infection_status = Progression::exposed;
  node.time_of_infection = time_of_infection;
  ++GLOBAL.INIT_ACTUALLY_INFECTED;
}

template <class T>
void set_node_initial_infection(agent& node,
								double community_infection_probability,
								int node_index,
								const T& elem,
								vector<count_type>& seed_candidates
								){
  if(SEED_INFECTION_FROM_FILE){
#ifdef DEBUG
	assert(elem["infection_status"].IsInt());
#endif
	if(elem["infection_status"].GetInt()){
	  seed_initial_infection_at_node(node, -elem["time_since_infected"].GetDouble());
	}
  }
  else {
	//Infections not being seeded from file
	bool seed_candidate =
	  (GLOBAL.SEED_HD_AREA_POPULATION || !node.hd_area_resident)
	  && !(GLOBAL.SEED_ONLY_NON_COMMUTER && node.has_to_travel);

	if(GLOBAL.SEED_FIXED_NUMBER){
	  if(seed_candidate){
		seed_candidates.push_back(node_index);
	  }
	}
	else {
	  if(seed_candidate
		 && bernoulli(community_infection_probability)){
		// Always seed non-high-density-ares residents
		// High-density-area residents seeded based on global flag.
		seed_initial_infection_at_node(node, -uniform_real(0, node.incubation_period));
	  }
	}
  }
  // node.infective = (node.infection_status == Progression::infective);
  
}

vector<agent> init_nodes(){
  auto indivJSON = readJSONFile(GLOBAL.input_base + "individuals.json");
  auto size = indivJSON.GetArray().Size();
  GLOBAL.num_people = size;
  vector<agent> nodes(size);
  auto community_infection_prob = compute_prob_infection_given_community(GLOBAL.INIT_FRAC_INFECTED, GLOBAL.USE_SAME_INFECTION_PROB_FOR_ALL_WARDS);

  count_type i = 0;

  vector<count_type> seed_candidates;
  seed_candidates.reserve(size);
  
  for (auto &elem: indivJSON.GetArray()){
 	nodes[i].loc = location{elem["lat"].GetDouble(),
							elem["lon"].GetDouble()};

#ifdef DEBUG
	assert(elem["age"].IsInt());
#endif
	int age = elem["age"].GetInt();
	nodes[i].age = age;
	nodes[i].age_group = get_age_group(age);
	nodes[i].age_index = get_age_index(age);
	nodes[i].zeta_a = zeta(age);

	nodes[i].infectiousness = gamma(GLOBAL.INFECTIOUSNESS_SHAPE,
									GLOBAL.INFECTIOUSNESS_SCALE);
	nodes[i].severity = bernoulli(GLOBAL.SEVERITY_RATE)?1:0;
	
#ifdef DEBUG
	assert(elem["household"].IsInt());
#endif
	nodes[i].home = elem["household"].GetInt();

	nodes[i].workplace = WORKPLACE_HOME; //null workplace, by default
	nodes[i].workplace_type = WorkplaceType::home; //home, by default

	if(elem["workplaceType"].IsInt()){
	  switch(elem["workplaceType"].GetInt()){
	  case 1:
		if(elem["workplace"].IsNumber()){
		  nodes[i].workplace_type = WorkplaceType::office;
		  nodes[i].workplace = int(elem["workplace"].GetDouble());
		}
		break;
	  case 2:
		if(elem["school"].IsNumber()){
		  nodes[i].workplace_type = WorkplaceType::school;
		  nodes[i].workplace = int(elem["school"].GetDouble());
		}
	  default:
		break;
	  }
	}
#ifdef DEBUG
	assert(elem["wardNo"].IsInt());
#endif
	count_type community = elem["wardNo"].GetInt() - 1;
	//minus 1 for 0-based indexing. PB: Might need to use
	//"wardIndex" instead, because that is the one actually sent by
	//the generator scripts.


	// Does the individual live in a slum?  In that case we need to
	// scale the contribution to their infection rates by a factor.
	//
	// Only use this feature if the field is present in the
	// "individuals" input files.
	if(elem.HasMember("slum")){

#ifdef DEBUG
	  assert(elem["slum"].IsInt());
#endif

	  if(elem["slum"].GetInt()){
		nodes[i].hd_area_resident = true;
		nodes[i].hd_area_factor = GLOBAL.HD_AREA_FACTOR;
		nodes[i].hd_area_exponent = GLOBAL.HD_AREA_EXPONENT;
	  }
	}
	
	nodes[i].community = community;
	nodes[i].funct_d_ck = f_kernel(elem["CommunityCentreDistance"].GetDouble());

	nodes[i].incubation_period = gamma(GLOBAL.INCUBATION_PERIOD_SHAPE,
									   GLOBAL.INCUBATION_PERIOD_SCALE);
	nodes[i].asymptomatic_period = gamma(1.0,
										 GLOBAL.ASYMPTOMATIC_PERIOD);
	nodes[i].symptomatic_period = gamma(1.0,
										GLOBAL.SYMPTOMATIC_PERIOD);
	
	nodes[i].hospital_regular_period = GLOBAL.HOSPITAL_REGULAR_PERIOD;
	nodes[i].hospital_critical_period = GLOBAL.HOSPITAL_CRITICAL_PERIOD;

	//Travel
	nodes[i].has_to_travel = bernoulli(GLOBAL.P_TRAIN);

	//Now procees node to check if it could be an initial seed given
	//all its other data
	set_node_initial_infection(nodes[i],
							   community_infection_prob[community],
							   i, elem,
							   seed_candidates);

	++i;
  }
  assert(i == GLOBAL.num_people);

  // If seeding a fixed number, go through the list of seed candidates
  // and seed a randomly chosen fixed number of them
  if(GLOBAL.SEED_FIXED_NUMBER){
	count_type candidate_list_size = seed_candidates.size();
	if (candidate_list_size > GLOBAL.INIT_FIXED_NUMBER_INFECTED){

	  //Randomly permute the list of candidates
	  std::shuffle(seed_candidates.begin(), seed_candidates.end(), GENERATOR);
	  
	}
	count_type num = std::min(candidate_list_size, GLOBAL.INIT_FIXED_NUMBER_INFECTED);
	for(count_type j = 0; j < num; ++j){
	  seed_initial_infection_at_node(nodes[seed_candidates[j]],
									 -uniform_real(0, nodes[seed_candidates[j]].incubation_period));
	}
  }
  return nodes;
}

matrix<double> compute_community_distances(const vector<community>& communities){
  auto wardDistJSON = readJSONFile(GLOBAL.input_base + "wardCentreDistance.json");
  const rapidjson::Value& mat = wardDistJSON.GetArray();
  auto size = mat.Size();
  matrix<double> dist_matrix(size, vector<double>(size));
  for(count_type i = 0; i < size; ++i){
	dist_matrix[i][i] = 0;
	for(count_type j = i + 1; j < size; ++j){
	  dist_matrix[i][j] = mat[i][to_string(j + 1).c_str()].GetDouble();
	  dist_matrix[j][i] = dist_matrix[i][j];
	}
  }
  return dist_matrix;
}

void assign_individual_home_community(vector<agent>& nodes, vector<house>& homes, vector<workplace>& workplaces, vector<community>& communities){
  //Assign individuals to homes, workplace, community
  for(count_type i = 0; i < nodes.size(); ++i){
	int home = nodes[i].home;
	homes[home].individuals.push_back(i);
	 //No checking for null as all individuals have a home
	nodes[i].compliant = homes[home].compliant;
	//All members of the household are set the same compliance value
	
	int workplace = nodes[i].workplace;
	if(workplace != WORKPLACE_HOME){
	  workplaces[workplace].individuals.push_back(i);

	  //Since the individual is not home_bound, compute their
	  //commute_distance
	  nodes[i].commute_distance
		= earth_distance(workplaces[workplace].loc,
						 homes[home].loc);
	}
	//No checking for null as all individuals have a community/ward
	communities[nodes[i].community].individuals.push_back(i);
  }
}

// Compute scale factors for each home, workplace and community. Done once at the beginning.
void compute_scale_homes(vector<house>& homes){
  for (count_type w = 0; w < homes.size(); ++w){
	if(homes[w].individuals.size()==0){
	  homes[w].scale = 0;
	} else {
	  homes[w].scale = GLOBAL.BETA_H
		* homes[w].Q_h
		/(pow(homes[w].individuals.size(), GLOBAL.ALPHA));
	}
  }
}

void compute_scale_workplaces(vector<workplace>& workplaces){
  for (count_type w=0; w < workplaces.size(); ++w) {
	if(workplaces[w].individuals.size()==0){
	  workplaces[w].scale = 0;
	} else {
	  double beta_workplace = 0;
	  if(workplaces[w].workplace_type == WorkplaceType::office){
		beta_workplace = GLOBAL.BETA_W; //workplace
	  } else if (workplaces[w].workplace_type == WorkplaceType::school){
		beta_workplace = GLOBAL.BETA_S; //school
	  }
	  workplaces[w].scale = beta_workplace
		* workplaces[w].Q_w
		/ workplaces[w].individuals.size();
	}
  }
}

void compute_scale_communities(const vector<agent>& nodes, vector<community>& communities){
  for (count_type w=0; w < communities.size(); ++w) {
	double sum_value = 0;
	for (auto indiv: communities[w].individuals){
	  sum_value += nodes[indiv].funct_d_ck;
	}
	if(sum_value==0){
	  communities[w].scale = 0;
	}
	else communities[w].scale = GLOBAL.BETA_C
		   * communities[w].Q_c / sum_value;
  }
}


