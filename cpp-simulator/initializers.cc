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
#include <cmath>
#include <set>

#include "models.h"
#include "initializers.h"

#ifdef DEBUG
#include <cassert>
#endif

using std::string;
using std::vector;
using std::set;
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
  double temp_non_compliance_metric = 0;
  count_type index = 0;

  bool compliance; 
  
  for (auto &elem: houseJSON.GetArray()){
    temp_non_compliance_metric = get_non_compliance_metric();
    if(elem.HasMember("slum") && elem["slum"].GetInt()){
	  compliance = (temp_non_compliance_metric<=GLOBAL.HD_COMPLIANCE_PROBABILITY);
	} else {
	  compliance = (temp_non_compliance_metric<=GLOBAL.COMPLIANCE_PROBABILITY);
	}
	homes[index].set(elem["lat"].GetDouble(),
					 elem["lon"].GetDouble(),
					 compliance,
					 temp_non_compliance_metric);

	//Cyclic strategy class
	if(GLOBAL.CYCLIC_POLICY_ENABLED && GLOBAL.CYCLIC_POLICY_TYPE == Cycle_Type::home){
	  homes[index].cyclic_strategy_class = uniform_count_type(0, GLOBAL.NUMBER_OF_CYCLIC_CLASSES - 1);
	}

    homes[index].age_independent_mixing = 0;
	if(GLOBAL.USE_AGE_DEPENDENT_MIXING){
	  homes[index].age_dependent_mixing.resize(GLOBAL.NUM_AGE_GROUPS, 0);
	}
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
    
    wps[index].age_independent_mixing = 0;
	if(GLOBAL.USE_AGE_DEPENDENT_MIXING){
	  wps[index].age_dependent_mixing.resize(GLOBAL.NUM_AGE_GROUPS, 0);
	}
    ++index;
  }
  assert(index == GLOBAL.num_schools);
  for (auto &elem: wpJSON.GetArray()){
	wps[index].set(elem["lat"].GetDouble(),
				   elem["lon"].GetDouble(),
				   WorkplaceType::office);

    wps[index].office_type = static_cast<OfficeType>(elem["officeType"].GetInt());
    wps[index].age_independent_mixing = 0;
	if(GLOBAL.USE_AGE_DEPENDENT_MIXING){
	  wps[index].age_dependent_mixing.resize(GLOBAL.NUM_AGE_GROUPS, 0);
	}
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

matrix<nbr_cell> init_nbr_cells() {

  matrix<nbr_cell> nbr_cells;

  if(GLOBAL.ENABLE_NBR_CELLS){
	location loc_temp;

	loc_temp.lat = GLOBAL.city_SW.lat;
	loc_temp.lon = GLOBAL.city_NE.lon;
	count_type num_x_grids = ceil(earth_distance(GLOBAL.city_SW,loc_temp)/GLOBAL.NBR_CELL_SIZE);
	nbr_cells.resize(num_x_grids);

	loc_temp.lon = GLOBAL.city_SW.lon;
	loc_temp.lat = GLOBAL.city_NE.lat;
	count_type num_y_grids = ceil(earth_distance(GLOBAL.city_SW,loc_temp)/GLOBAL.NBR_CELL_SIZE);

	for(count_type count_x_grid = 0; count_x_grid < num_x_grids; ++count_x_grid){
		nbr_cells[count_x_grid].resize(num_y_grids);
		for(count_type count_y_grid = 0; count_y_grid < num_y_grids; ++count_y_grid){
			nbr_cells[count_x_grid][count_y_grid].neighbourhood.cell_x = count_x_grid;
			nbr_cells[count_x_grid][count_y_grid].neighbourhood.cell_y = count_y_grid;
		}
	} 
  }  
  return nbr_cells;
}

void print_intervention_params(const int index, const intervention_params intv_params){
	std::cout<<std::endl<<"Index : "<<index<<". num_days = "<<	intv_params.num_days;
	std::cout<<". Case Isolation : " << intv_params.case_isolation;
	std::cout<<". Home Quarantine : " << intv_params.home_quarantine;
	std::cout<<". Lockdown : " << intv_params.lockdown;
	std::cout<<". SDO : " << intv_params.social_dist_elderly;
	std::cout<<". School Closed : " << intv_params.school_closed;
	std::cout<<". workplace_odd_even : " << intv_params.workplace_odd_even;
	std::cout<<". SC_factor : " << intv_params.SC_factor;
	std::cout<<". community_factor : " << intv_params.community_factor;
	std::cout<<". neighbourhood_containment : " << intv_params.neighbourhood_containment;
	std::cout<<". ward_containment : " << intv_params.ward_containment;
	std::cout<<". compliance : " << intv_params.compliance;
	std::cout<<". compliance_hd : " << intv_params.compliance_hd;
	std::cout<<". trains_active : " << intv_params.trains_active;
	std::cout<<". fraction_forced_to_take_train : " << intv_params.fraction_forced_to_take_train;
}

vector<intervention_params> init_intervention_params(){
  vector<intervention_params> intv_params;
  if(GLOBAL.INTERVENTION==Intervention::intv_file_read){
	std::cout<<std::endl<<"Inside init_intervention_params";
	auto intvJSON = readJSONFile(GLOBAL.input_base + GLOBAL.intervention_filename);

	intv_params.reserve(intvJSON.GetArray().Size());

	int index = 0;
	for (auto &elem: intvJSON.GetArray()){
	  intervention_params temp;
	  if((elem.HasMember("num_days")) && (elem["num_days"].GetInt() > 0)){
		temp.num_days = elem["num_days"].GetInt();
		if(elem.HasMember("compliance")){
		  temp.compliance = elem["compliance"].GetDouble();
          temp.compliance_hd = elem["compliance"].GetDouble();
          //By default, compliance = compliance_hd. Can be reset below
		} else{
		  temp.compliance = GLOBAL.COMPLIANCE_PROBABILITY;
          temp.compliance_hd = GLOBAL.COMPLIANCE_PROBABILITY;
          //By default, compliance = compliance_hd. Can be reset below
		}
        if(elem.HasMember("compliance_hd")){
		  temp.compliance_hd = elem["compliance_hd"].GetDouble();
		} else{
		  temp.compliance_hd = GLOBAL.COMPLIANCE_PROBABILITY;
		}
		if(elem.HasMember("case_isolation")){
		  temp.case_isolation = elem["case_isolation"]["active"].GetBool();
		}
		if(elem.HasMember("home_quarantine")){
		  temp.home_quarantine = elem["home_quarantine"]["active"].GetBool();
		}
		if(elem.HasMember("lockdown")){
		  //TODO: collect all these statements in a function.
		  temp.lockdown = elem["lockdown"]["active"].GetBool();
		  if(elem["lockdown"].HasMember("kappa_values_compliant")){
			  if(elem["lockdown"]["kappa_values_compliant"].HasMember("kappa_H")){
				  temp.lockdown_kappas_compliant.kappa_H = elem["lockdown"]["kappa_values_compliant"]["kappa_H"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_compliant"].HasMember("kappa_H_incoming")){
				  temp.lockdown_kappas_compliant.kappa_H_incoming = elem["lockdown"]["kappa_values_compliant"]["kappa_H_incoming"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_compliant"].HasMember("kappa_W")){
				  temp.lockdown_kappas_compliant.kappa_W = elem["lockdown"]["kappa_values_compliant"]["kappa_W"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_compliant"].HasMember("kappa_W_incoming")){
				  temp.lockdown_kappas_compliant.kappa_W_incoming = elem["lockdown"]["kappa_values_compliant"]["kappa_W_incoming"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_compliant"].HasMember("kappa_C")){
				  temp.lockdown_kappas_compliant.kappa_C = elem["lockdown"]["kappa_values_compliant"]["kappa_C"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_compliant"].HasMember("kappa_C_incoming")){
				  temp.lockdown_kappas_compliant.kappa_C_incoming = elem["lockdown"]["kappa_values_compliant"]["kappa_C_incoming"].GetDouble();
			  }			  	
		  }
		  if(elem["lockdown"].HasMember("kappa_values_non_compliant")){
			  if(elem["lockdown"]["kappa_values_non_compliant"].HasMember("kappa_H")){
				  temp.lockdown_kappas_non_compliant.kappa_H = elem["lockdown"]["kappa_values_non_compliant"]["kappa_H"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_non_compliant"].HasMember("kappa_H_incoming")){
				  temp.lockdown_kappas_non_compliant.kappa_H_incoming = elem["lockdown"]["kappa_values_non_compliant"]["kappa_H_incoming"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_non_compliant"].HasMember("kappa_W")){
				  temp.lockdown_kappas_non_compliant.kappa_W = elem["lockdown"]["kappa_values_non_compliant"]["kappa_W"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_non_compliant"].HasMember("kappa_W_incoming")){
				  temp.lockdown_kappas_non_compliant.kappa_W_incoming = elem["lockdown"]["kappa_values_non_compliant"]["kappa_W_incoming"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_non_compliant"].HasMember("kappa_C")){
				  temp.lockdown_kappas_non_compliant.kappa_C = elem["lockdown"]["kappa_values_non_compliant"]["kappa_C"].GetDouble();
			  }
			  if(elem["lockdown"]["kappa_values_non_compliant"].HasMember("kappa_C_incoming")){
				  temp.lockdown_kappas_non_compliant.kappa_C_incoming = elem["lockdown"]["kappa_values_non_compliant"]["kappa_C_incoming"].GetDouble();
			  }			  	
		  }
		}
		if(elem.HasMember("social_dist_elderly")){
		  temp.social_dist_elderly = elem["social_dist_elderly"]["active"].GetBool();
		}
		if(elem.HasMember("school_closed")){
		  temp.school_closed = elem["school_closed"]["active"].GetBool();
		  if(elem["school_closed"].HasMember("SC_factor")){
			temp.SC_factor = elem["school_closed"]["SC_factor"].GetDouble();
		  }
		}
		if(elem.HasMember("workplace_odd_even")){
		  temp.workplace_odd_even = elem["workplace_odd_even"]["active"].GetBool();
		}
		if(elem.HasMember("community_factor")){
		  temp.community_factor = elem["community_factor"].GetDouble();
		}
        if(elem.HasMember("trains")){
          temp.trains_active = elem["trains"]["active"].GetBool();
          if(elem["trains"].HasMember("fraction_forced_to_take_train")){
            temp.fraction_forced_to_take_train = elem["trains"]["fraction_forced_to_take_train"].GetDouble();
          }
        }
		if(elem.HasMember("neighbourhood_containment")){
		  temp.neighbourhood_containment = elem["neighbourhood_containment"]["active"].GetBool() && GLOBAL.ENABLE_CONTAINMENT;
		  if(!GLOBAL.ENABLE_CONTAINMENT){
			std::cout<<std::endl<<"To enable containment strategies, add  --ENABLE_CONTAINMENT to argument list. Ignoring neighbourhood containment.";
		  }
		}
		if(elem.HasMember("ward_containment")){
		  temp.ward_containment = elem["ward_containment"]["active"].GetBool() && GLOBAL.ENABLE_CONTAINMENT;
		  if(!GLOBAL.ENABLE_CONTAINMENT){
			std::cout<<std::endl<<"To enable containment strategies, add  --ENABLE_CONTAINMENT to argument list. Ignoring ward containment.";
		  }
		}
		print_intervention_params(index, temp);
		intv_params.push_back(temp);
		++index;
	  }else{
		std::cout<<std::endl<<"num_days not specified or less than 1. Skipping current index.";
		assert(false);
	  }
	}
  }
  std::cout<<std::endl<<"Intervention params size = "<<intv_params.size();
  return intv_params;
}

void print_testing_protocol(const int index, const testing_probability probabilities){
  std::cout<<std::endl<<"Index : "<<index<<". num_days = "<<probabilities.num_days;
  std::cout<<".  prob_test_index_symptomatic:  "<<probabilities.prob_test_index_symptomatic;
  std::cout<<".  prob_test_index_hospitalised:  "<<probabilities.prob_test_index_hospitalised;

  std::cout<<".  prob_test_household_positive_symptomatic:  "<<probabilities.prob_test_household_positive_symptomatic;
  std::cout<<".  prob_test_household_hospitalised_symptomatic:  "<<probabilities.prob_test_household_hospitalised_symptomatic;
  std::cout<<".  prob_test_household_symptomatic_symptomatic:  "<<probabilities.prob_test_household_symptomatic_symptomatic;
  std::cout<<".  prob_test_household_positive_asymptomatic:  "<<probabilities.prob_test_household_positive_asymptomatic;
  std::cout<<".  prob_test_household_hospitalised_asymptomatic:  "<<probabilities.prob_test_household_hospitalised_asymptomatic;
  std::cout<<".  prob_test_household_symptomatic_asymptomatic:  "<<probabilities.prob_test_household_symptomatic_asymptomatic;

  std::cout<<".  prob_test_workplace_positive_symptomatic:  "<<probabilities.prob_test_workplace_positive_symptomatic;
  std::cout<<".  prob_test_workplace_hospitalised_symptomatic:  "<<probabilities.prob_test_workplace_hospitalised_symptomatic;
  std::cout<<".  prob_test_workplace_symptomatic_symptomatic:  "<<probabilities.prob_test_workplace_symptomatic_symptomatic;
  std::cout<<".  prob_test_workplace_positive_asymptomatic:  "<<probabilities.prob_test_workplace_positive_asymptomatic;
  std::cout<<".  prob_test_workplace_hospitalised_asymptomatic:  "<<probabilities.prob_test_workplace_hospitalised_asymptomatic;
  std::cout<<".  prob_test_workplace_symptomatic_asymptomatic:  "<<probabilities.prob_test_workplace_symptomatic_asymptomatic;
  
  std::cout<<".  prob_test_random_community_positive_symptomatic:  "<<probabilities.prob_test_random_community_positive_symptomatic;
  std::cout<<".  prob_test_random_community_hospitalised_symptomatic:  "<<probabilities.prob_test_random_community_hospitalised_symptomatic;
  std::cout<<".  prob_test_random_community_symptomatic_symptomatic:  "<<probabilities.prob_test_random_community_symptomatic_symptomatic;
  std::cout<<".  prob_test_random_community_positive_asymptomatic:  "<<probabilities.prob_test_random_community_positive_asymptomatic;
  std::cout<<".  prob_test_random_community_hospitalised_asymptomatic:  "<<probabilities.prob_test_random_community_hospitalised_asymptomatic;
  std::cout<<".  prob_test_random_community_symptomatic_asymptomatic:  "<<probabilities.prob_test_random_community_symptomatic_asymptomatic;

  std::cout<<".  prob_test_neighbourhood_positive_symptomatic:  "<<probabilities.prob_test_neighbourhood_positive_symptomatic;
  std::cout<<".  prob_test_neighbourhood_hospitalised_symptomatic:  "<<probabilities.prob_test_neighbourhood_hospitalised_symptomatic;
  std::cout<<".  prob_test_neighbourhood_symptomatic_symptomatic:  "<<probabilities.prob_test_neighbourhood_symptomatic_symptomatic;
  std::cout<<".  prob_test_neighbourhood_positive_asymptomatic:  "<<probabilities.prob_test_neighbourhood_positive_asymptomatic;
  std::cout<<".  prob_test_neighbourhood_hospitalised_asymptomatic:  "<<probabilities.prob_test_neighbourhood_hospitalised_asymptomatic;
  std::cout<<".  prob_test_neighbourhood_symptomatic_asymptomatic:  "<<probabilities.prob_test_neighbourhood_symptomatic_asymptomatic;
 
  std::cout<<".  prob_test_school_positive_symptomatic:  "<<probabilities.prob_test_school_positive_symptomatic;
  std::cout<<".  prob_test_school_hospitalised_symptomatic:  "<<probabilities.prob_test_school_hospitalised_symptomatic;
  std::cout<<".  prob_test_school_symptomatic_symptomatic:  "<<probabilities.prob_test_school_symptomatic_symptomatic;
  std::cout<<".  prob_test_school_positive_asymptomatic:  "<<probabilities.prob_test_school_positive_asymptomatic;
  std::cout<<".  prob_test_school_hospitalised_asymptomatic:  "<<probabilities.prob_test_school_hospitalised_asymptomatic;
  std::cout<<".  prob_test_school_symptomatic_asymptomatic:  "<<probabilities.prob_test_school_symptomatic_asymptomatic;
 
  std::cout<<".  prob_retest_recovered:  "<<probabilities.prob_retest_recovered;
  
  std::cout<<".  prob_contact_trace_household_symptomatic:  "<<probabilities.prob_contact_trace_household_symptomatic;
  std::cout<<".  prob_contact_trace_project_symptomatic:  "<<probabilities.prob_contact_trace_project_symptomatic;
  std::cout<<".  prob_contact_trace_random_community_symptomatic:  "<<probabilities.prob_contact_trace_random_community_symptomatic;
  std::cout<<".  prob_contact_trace_neighbourhood_symptomatic:  "<<probabilities.prob_contact_trace_neighbourhood_symptomatic;
  std::cout<<".  prob_contact_trace_class_symptomatic:  "<<probabilities.prob_contact_trace_class_symptomatic;

  std::cout<<".  prob_contact_trace_household_hospitalised:  "<<probabilities.prob_contact_trace_household_hospitalised;
  std::cout<<".  prob_contact_trace_project_hospitalised:  "<<probabilities.prob_contact_trace_project_hospitalised;
  std::cout<<".  prob_contact_trace_random_community_hospitalised:  "<<probabilities.prob_contact_trace_random_community_hospitalised;
  std::cout<<".  prob_contact_trace_neighbourhood_hospitalised:  "<<probabilities.prob_contact_trace_neighbourhood_hospitalised;
  std::cout<<".  prob_contact_trace_class_hospitalised:  "<<probabilities.prob_contact_trace_class_hospitalised;

  std::cout<<".  prob_contact_trace_household_positive:  "<<probabilities.prob_contact_trace_household_positive;
  std::cout<<".  prob_contact_trace_project_positive:  "<<probabilities.prob_contact_trace_project_positive;
  std::cout<<".  prob_contact_trace_random_community_positive:  "<<probabilities.prob_contact_trace_random_community_positive;
  std::cout<<".  prob_contact_trace_neighbourhood_positive:  "<<probabilities.prob_contact_trace_neighbourhood_positive;
  std::cout<<".  prob_contact_trace_class_positive:  "<<probabilities.prob_contact_trace_class_positive;

}
  
vector<testing_probability> init_testing_protocol(){
  vector<testing_probability> testing_protocol;
  if(GLOBAL.TESTING_PROTOCOL==Testing_Protocol::testing_protocol_file_read){
	std::cout<<std::endl<<"Inside init_testing_protocol";
	auto testProtJSON = readJSONFile(GLOBAL.input_base + GLOBAL.testing_protocol_filename);

	testing_protocol.reserve(testProtJSON.GetArray().Size());
	count_type index = 0;
	for (auto &elem: testProtJSON.GetArray()){
	  testing_probability temp;
	  if((elem.HasMember("num_days")) && (elem["num_days"].GetInt() > 0)){
		temp.num_days = elem["num_days"].GetInt();
		if(elem.HasMember("test_false_positive")){
		  GLOBAL.TEST_FALSE_POSITIVE = elem["test_false_positive"].GetDouble();
		}
		if(elem.HasMember("test_false_negative")){
		  GLOBAL.TEST_FALSE_NEGATIVE = elem["test_false_negative"].GetDouble();
		}
		if(elem.HasMember("prob_test_index_symptomatic")){
		  temp.prob_test_index_symptomatic = elem["prob_test_index_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_index_hospitalised")){
		  temp.prob_test_index_hospitalised = elem["prob_test_index_hospitalised"].GetDouble();
		}
		
		//Testing probabilities for household networks
		if(elem.HasMember("prob_test_household_positive_symptomatic")){
		  temp.prob_test_household_positive_symptomatic = elem["prob_test_household_positive_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_household_hospitalised_symptomatic")){
		  temp.prob_test_household_hospitalised_symptomatic = elem["prob_test_household_hospitalised_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_household_symptomatic_symptomatic")){
		  temp.prob_test_household_symptomatic_symptomatic = elem["prob_test_household_symptomatic_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_household_positive_asymptomatic")){
		  temp.prob_test_household_positive_asymptomatic = elem["prob_test_household_positive_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_household_hospitalised_asymptomatic")){
		  temp.prob_test_household_hospitalised_asymptomatic = elem["prob_test_household_hospitalised_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_household_symptomatic_asymptomatic")){
		  temp.prob_test_household_symptomatic_asymptomatic = elem["prob_test_household_symptomatic_asymptomatic"].GetDouble();
		}

		//Testing probabilities for workplace networks
		if(elem.HasMember("prob_test_workplace_positive_symptomatic")){
		  temp.prob_test_workplace_positive_symptomatic = elem["prob_test_workplace_positive_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_workplace_hospitalised_symptomatic")){
		  temp.prob_test_workplace_hospitalised_symptomatic = elem["prob_test_workplace_hospitalised_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_workplace_symptomatic_symptomatic")){
		  temp.prob_test_workplace_symptomatic_symptomatic = elem["prob_test_workplace_symptomatic_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_workplace_positive_asymptomatic")){
		  temp.prob_test_workplace_positive_asymptomatic = elem["prob_test_workplace_positive_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_workplace_hospitalised_asymptomatic")){
		  temp.prob_test_workplace_hospitalised_asymptomatic = elem["prob_test_workplace_hospitalised_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_workplace_symptomatic_asymptomatic")){
		  temp.prob_test_workplace_symptomatic_asymptomatic = elem["prob_test_workplace_symptomatic_asymptomatic"].GetDouble();
		}

		//Testing probabilities for school networks
		if(elem.HasMember("prob_test_school_positive_symptomatic")){
		  temp.prob_test_school_positive_symptomatic = elem["prob_test_school_positive_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_school_hospitalised_symptomatic")){
		  temp.prob_test_school_hospitalised_symptomatic = elem["prob_test_school_hospitalised_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_school_symptomatic_symptomatic")){
		  temp.prob_test_school_symptomatic_symptomatic = elem["prob_test_school_symptomatic_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_school_positive_asymptomatic")){
		  temp.prob_test_school_positive_asymptomatic = elem["prob_test_school_positive_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_school_hospitalised_asymptomatic")){
		  temp.prob_test_school_hospitalised_asymptomatic = elem["prob_test_school_hospitalised_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_school_symptomatic_asymptomatic")){
		  temp.prob_test_school_symptomatic_asymptomatic = elem["prob_test_school_symptomatic_asymptomatic"].GetDouble();
		}

		//Testing probabilities for random community networks
		if(elem.HasMember("prob_test_random_community_positive_symptomatic")){
		  temp.prob_test_random_community_positive_symptomatic = elem["prob_test_random_community_positive_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_random_community_hospitalised_symptomatic")){
		  temp.prob_test_random_community_hospitalised_symptomatic = elem["prob_test_random_community_hospitalised_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_random_community_symptomatic_symptomatic")){
		  temp.prob_test_random_community_symptomatic_symptomatic = elem["prob_test_random_community_symptomatic_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_random_community_positive_asymptomatic")){
		  temp.prob_test_random_community_positive_asymptomatic = elem["prob_test_random_community_positive_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_random_community_hospitalised_asymptomatic")){
		  temp.prob_test_random_community_hospitalised_asymptomatic = elem["prob_test_random_community_hospitalised_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_random_community_symptomatic_asymptomatic")){
		  temp.prob_test_random_community_symptomatic_asymptomatic = elem["prob_test_random_community_symptomatic_asymptomatic"].GetDouble();
		}

		//Testing probabilities for random community networks
		if(elem.HasMember("prob_test_neighbourhood_positive_symptomatic")){
		  temp.prob_test_neighbourhood_positive_symptomatic = elem["prob_test_neighbourhood_positive_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_neighbourhood_hospitalised_symptomatic")){
		  temp.prob_test_neighbourhood_hospitalised_symptomatic = elem["prob_test_neighbourhood_hospitalised_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_neighbourhood_symptomatic_symptomatic")){
		  temp.prob_test_neighbourhood_symptomatic_symptomatic = elem["prob_test_neighbourhood_symptomatic_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_neighbourhood_positive_asymptomatic")){
		  temp.prob_test_neighbourhood_positive_asymptomatic = elem["prob_test_neighbourhood_positive_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_neighbourhood_hospitalised_asymptomatic")){
		  temp.prob_test_neighbourhood_hospitalised_asymptomatic = elem["prob_test_neighbourhood_hospitalised_asymptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_test_neighbourhood_symptomatic_asymptomatic")){
		  temp.prob_test_neighbourhood_symptomatic_asymptomatic = elem["prob_test_neighbourhood_symptomatic_asymptomatic"].GetDouble();
		}

		//Contact trace probabilities for in networks for symptomatic index patient.
		if(elem.HasMember("prob_contact_trace_household_symptomatic")){
		  temp.prob_contact_trace_household_symptomatic = elem["prob_contact_trace_household_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_project_symptomatic")){
		  temp.prob_contact_trace_project_symptomatic = elem["prob_contact_trace_project_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_random_community_symptomatic")){
		  temp.prob_contact_trace_random_community_symptomatic = elem["prob_contact_trace_random_community_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_neighbourhood_symptomatic")){
		  temp.prob_contact_trace_neighbourhood_symptomatic = elem["prob_contact_trace_neighbourhood_symptomatic"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_class_symptomatic")){
		  temp.prob_contact_trace_class_symptomatic = elem["prob_contact_trace_class_symptomatic"].GetDouble();
		}

		//Contact trace probabilities for in networks for hospitalised index patient.
		if(elem.HasMember("prob_contact_trace_household_hospitalised")){
		  temp.prob_contact_trace_household_hospitalised = elem["prob_contact_trace_household_hospitalised"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_project_hospitalised")){
		  temp.prob_contact_trace_project_hospitalised = elem["prob_contact_trace_project_hospitalised"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_random_community_hospitalised")){
		  temp.prob_contact_trace_random_community_hospitalised = elem["prob_contact_trace_random_community_hospitalised"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_neighbourhood_hospitalised")){
		  temp.prob_contact_trace_neighbourhood_hospitalised = elem["prob_contact_trace_neighbourhood_hospitalised"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_class_hospitalised")){
		  temp.prob_contact_trace_class_hospitalised = elem["prob_contact_trace_class_hospitalised"].GetDouble();
		}

		//Contact trace probabilities for in networks for positive index patient.
		if(elem.HasMember("prob_contact_trace_household_positive")){
		  temp.prob_contact_trace_household_positive = elem["prob_contact_trace_household_positive"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_project_positive")){
		  temp.prob_contact_trace_project_positive = elem["prob_contact_trace_project_positive"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_random_community_positive")){
		  temp.prob_contact_trace_random_community_positive = elem["prob_contact_trace_random_community_positive"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_neighbourhood_positive")){
		  temp.prob_contact_trace_neighbourhood_positive = elem["prob_contact_trace_neighbourhood_positive"].GetDouble();
		}
		if(elem.HasMember("prob_contact_trace_class_positive")){
		  temp.prob_contact_trace_class_positive = elem["prob_contact_trace_class_positive"].GetDouble();
		}


		if(elem.HasMember("prob_retest_recovered")){
		  temp.prob_retest_recovered = elem["prob_retest_recovered"].GetDouble();
		}

		testing_protocol.push_back(temp);
		print_testing_protocol(index, temp);
		++index;
	  }else{
		std::cout<<std::endl<<"num_days not specified or less than 1. Skipping current index.";
		assert(false);
	  }
	}
  }
  std::cout<<std::endl<<"Intervention params size = "<<testing_protocol.size();
  return testing_protocol;
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
	nodes[i].workplace_subnetwork = 0;

	if(elem["workplaceType"].IsInt()){
	  switch(elem["workplaceType"].GetInt()){
	  case 1:
		if(elem["workplace"].IsNumber()){
		  nodes[i].workplace_type = WorkplaceType::office;
		  nodes[i].workplace = int(elem["workplace"].GetDouble());
		  //Travel
		  nodes[i].has_to_travel = bernoulli(GLOBAL.P_TRAIN);
		}
		break;
	  case 2:
		if(elem["school"].IsNumber()){
		  nodes[i].workplace_type = WorkplaceType::school;
		  nodes[i].workplace = int(elem["school"].GetDouble());
		  //Travel
		  nodes[i].has_to_travel = bernoulli(GLOBAL.P_TRAIN);
		  nodes[i].workplace_subnetwork = age;
		}
		break;
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

	//Now procees node to check if it could be an initial seed given
	//all its other data
	set_node_initial_infection(nodes[i],
							   community_infection_prob[community],
							   i, elem,
							   seed_candidates);

	nodes[i].test_status.tested_epoch = -1*GLOBAL.MINIMUM_TEST_INTERVAL*GLOBAL.SIM_STEPS_PER_DAY;
	
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

vector<double> read_JSON_convert_array(const string& file_name){
  vector<double> return_object;
  auto file_JSON = readJSONFile(GLOBAL.input_base + "age_tx/" + file_name);
  auto size = file_JSON.GetArray().Size();
  return_object.resize(size);
  int i = 0;
  for (auto &elem: file_JSON.GetArray()){
    return_object[i] = elem[to_string(i).c_str()].GetDouble();
    i += 1;
  }
  return return_object;  
}

matrix<double> read_JSON_convert_matrix(const string& file_name){ 
  matrix<double> return_object;
  auto file_JSON = readJSONFile(GLOBAL.input_base + "age_tx/" + file_name);
  auto size = file_JSON.GetArray().Size();
  return_object.resize(size, vector<double>(size));
  int i = 0;
  for (auto &elem: file_JSON.GetArray()){
    for (count_type j = 0; j < size; ++j){
       return_object[i][j] = elem[to_string(j).c_str()].GetDouble();
    }
    i += 1;
  }
  return return_object; 
}

void init_age_interaction_matrix(const string& directory_base, svd& svd){
  string u_file = directory_base + "/U_" + directory_base + ".json";
  string sigma_file = directory_base + "/Sigma_" + directory_base + ".json";
  string vT_file = directory_base + "/Vtranspose_" + directory_base + ".json";

  svd.u = read_JSON_convert_matrix(u_file);
  svd.sigma = read_JSON_convert_array(sigma_file);
  svd.vT = read_JSON_convert_matrix(vT_file);
}

svd init_home_age_interaction_matrix(){
  svd svd;
  init_age_interaction_matrix("home", svd);
  return svd;
}

svd init_school_age_interaction_matrix(){
  svd svd;
  init_age_interaction_matrix("school", svd);
  return svd;
}

svd init_workplace_age_interaction_matrix(){
  svd svd;
  init_age_interaction_matrix("workplace", svd);
  return svd;
}

svd init_community_age_interaction_matrix(){
  svd svd;
  init_age_interaction_matrix("other", svd);
  return svd;
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

matrix<double> compute_community_distances_fkernel(const matrix<double>& community_distances){
  auto size = community_distances.size();
  matrix<double> fk_matrix(size, vector<double>(size));
  for(count_type i = 0; i < size; ++i){
	fk_matrix[i][i] = f_kernel(community_distances[i][i]);
	for(count_type j = i + 1; j < size; ++j){
	  fk_matrix[i][j] = f_kernel(community_distances[i][j]);
	  fk_matrix[j][i] = fk_matrix[i][j];
	}
  }
  return fk_matrix;
}

void assign_individual_home_community(vector<agent>& nodes, vector<house>& homes, vector<workplace>& workplaces, vector<community>& communities){
  //Assign individuals to homes, workplace, community
  for(count_type i = 0; i < nodes.size(); ++i){
	int home = nodes[i].home;
	homes[home].individuals.push_back(i);
	 //No checking for null as all individuals have a home
	nodes[i].compliant = homes[home].compliant;
	//All members of the household are set the same compliance value

	//Assign cyclic strategy class for this node
	if(GLOBAL.CYCLIC_POLICY_ENABLED){
	  switch(GLOBAL.CYCLIC_POLICY_TYPE){
	  case Cycle_Type::home:
		nodes[i].cyclic_strategy_class = homes[home].cyclic_strategy_class;
		break;
	  case Cycle_Type::individual:
		nodes[i].cyclic_strategy_class = uniform_count_type(0, GLOBAL.NUMBER_OF_CYCLIC_CLASSES - 1);
		break;
	  default:
		assert(false);
		break;
	  }
	  assert(0 <= nodes[i].cyclic_strategy_class &&
			 nodes[i].cyclic_strategy_class < GLOBAL.NUMBER_OF_CYCLIC_CLASSES);
	}
	
	int workplace = nodes[i].workplace;
    if(nodes[i].workplace_type == WorkplaceType::office){
      nodes[i].office_type = workplaces[workplace].office_type;
    }
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

void assign_individual_projects(vector<workplace>& workplaces, vector<agent>& nodes){
  for(count_type i=0; i<workplaces.size(); ++i){
	if(workplaces[i].workplace_type==WorkplaceType::office){
	  randomly_shuffle(workplaces[i].individuals);
	  count_type shuffle_index = 0, project_index = 0;
	  count_type individuals_to_be_assigned = workplaces[i].individuals.size();
	  while(individuals_to_be_assigned > 0){
		project temp;
		workplaces[i].projects.push_back(temp);
		workplaces[i].projects[project_index].workplace = i;
		count_type project_size = uniform_count_type_network(GLOBAL.MIN_PROJECT_SIZE, GLOBAL.MAX_PROJECT_SIZE);
		project_size = std::min(project_size, individuals_to_be_assigned);
		for(count_type j=0; j<project_size; ++j){
		  auto individual = workplaces[i].individuals[shuffle_index];
		  workplaces[i].projects[project_index].individuals.push_back(individual);
		  nodes[individual].workplace_subnetwork = project_index;
		  ++shuffle_index;
		}
		++project_index;
		individuals_to_be_assigned -= project_size;
	  }
	}
	if(workplaces[i].workplace_type==WorkplaceType::school){
	  workplaces[i].projects.resize(GLOBAL.MAX_CLASS_AGE+1);
	  //for(count_type j=0; j < workplaces[i].individuals.size(); ++j){
	  for(const auto& individual: workplaces[i].individuals){
		auto age_index = nodes[individual].workplace_subnetwork;
		//For agents whose workplace is school, workplace_subnetwork is already
		//initialized in init_nodes(), and is equal to the age of the individual
		workplaces[i].projects[age_index].individuals.push_back(individual);
	  }
	}
  }
}


void assign_household_community(vector<community>& communities, const vector<agent>& nodes, vector<house>& homes){
  for(count_type i=0; i<homes.size(); ++i){
	if(homes[i].individuals.size()>0){
	  homes[i].community = nodes[homes[i].individuals[0]].community;
	  //All individuals in the same home have the same community, so we can take
	  //any one.
	  communities[homes[i].community].households.push_back(i);
	}	      
  }
}


void assign_household_random_community(vector<house>& homes, const vector<community>& communities){
  for(count_type i = 0; i < communities.size(); ++i){
	auto NUM_HOUSEHOLDS = communities[i].households.size();
	for(count_type j = 0; j < NUM_HOUSEHOLDS; ++j){
	  count_type current_household = communities[i].households[j];
	  count_type degree = uniform_count_type_network(GLOBAL.MIN_RANDOM_COMMUNITY_SIZE/2,
													 GLOBAL.MAX_RANDOM_COMMUNITY_SIZE/2);
	  degree = (degree > NUM_HOUSEHOLDS-1)?NUM_HOUSEHOLDS-1:degree;
	  count_type candidate;
	  while(homes[current_household].random_households.households.size() < degree){
		do{
		  candidate = communities[i].households[uniform_count_type_network(0, NUM_HOUSEHOLDS - 1)];
		}while(candidate == current_household
			   || (std::find(
							 homes[current_household].random_households.households.begin(),
							 homes[current_household].random_households.households.end(),
							 candidate
							 )
				   != homes[current_household].random_households.households.end()));
		homes[current_household].random_households.households.push_back(candidate);
		//homes[candidate].random_households.households.push_back(current_household);
	  }
	}
  }
  //symmetrize random network interaction graph. 
  for(count_type current_house = 0; current_house < homes.size(); ++current_house){
	for (count_type j = 0; j < homes[current_house].random_households.households.size(); ++j){
	  count_type random_contact = homes[current_house].random_households.households[j];
	  if(std::find(homes[random_contact].random_households.households.begin(),
				   homes[random_contact].random_households.households.end(),
				   current_house)
		 == homes[random_contact].random_households.households.end()){
		homes[random_contact].random_households.households.push_back(current_house);
	  }
	}
  }
}


void assign_homes_nbr_cell(const vector<house>& homes, matrix<nbr_cell>& neighbourhood_cells){
	if(!GLOBAL.ENABLE_NBR_CELLS){
		return;
	}
	for (count_type home_count = 0; home_count < homes.size(); ++home_count){
		grid_cell my_nbr_cell = homes[home_count].neighbourhood;
		neighbourhood_cells[my_nbr_cell.cell_x][my_nbr_cell.cell_y].houses_list.push_back(home_count);
		neighbourhood_cells[my_nbr_cell.cell_x][my_nbr_cell.cell_y].population
		  += homes[home_count].individuals.size();
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
	for(count_type j=0; j<workplaces[w].projects.size(); ++j){
	  if(workplaces[w].projects[j].individuals.size() ==0){
		workplaces[w].projects[j].scale = 0;
	  }
	  else{
		double beta_project = 0;
		if(workplaces[w].workplace_type == WorkplaceType::office){
		  beta_project = GLOBAL.BETA_PROJECT; //project
		} else if (workplaces[w].workplace_type == WorkplaceType::school){
		  beta_project = GLOBAL.BETA_CLASS; // class
		}
		workplaces[w].projects[j].scale = beta_project/workplaces[w].projects[j].individuals.size();
	  }
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

void compute_scale_random_community(vector<house>& houses, vector<agent>& nodes){
  for(count_type i=0; i<houses.size(); ++i){
  	double sum_value = 0;
  	for(count_type j=0; j < houses[i].random_households.households.size(); ++j){
	  auto neighbor = houses[i].random_households.households[j];
	  if(houses[neighbor].individuals.size()>0){
		sum_value += houses[neighbor].individuals.size()*nodes[houses[neighbor].individuals[0]].funct_d_ck;
	  }
	}
	if(sum_value==0){
	  houses[i].random_households.scale = 0;
	}
	else{
	  houses[i].random_households.scale = GLOBAL.BETA_RANDOM_COMMUNITY/sum_value;
	}
  }
}

void compute_scale_nbr_cells(vector<agent>& nodes, vector<vector<nbr_cell>>& nbr_cells, const vector<house>& homes){
  for(count_type i=0; i<nbr_cells.size(); ++i){
	for(count_type j=0; j<nbr_cells[i].size(); ++j){
		double sum_values = 0;
		for(count_type h=0; h<nbr_cells[i][j].houses_list.size(); ++h){
		  sum_values += homes[nbr_cells[i][j].houses_list[h]].individuals.size();
		}
		if(sum_values>0){
			nbr_cells[i][j].scale = GLOBAL.BETA_NBR_CELLS/sum_values;
		}
		else{
			nbr_cells[i][j].scale = 0;
		}
	}
  }
}


//Initialize the office attendance
void initialize_office_attendance(){
  //No need to read the attendance file if we are specifically asked to ignore
  //it
  if(GLOBAL.IGNORE_ATTENDANCE_FILE) return;

  //constexpr count_type NUMBER_OF_OFFICE_TYPES = 6;
  auto attendanceJSON = readJSONFile(GLOBAL.input_base + GLOBAL.attendance_filename);
  ATTENDANCE.number_of_entries = attendanceJSON.GetArray().Size(); //will change for new file type
  ATTENDANCE.probabilities.reserve(ATTENDANCE.number_of_entries); //will change for new file type
  count_type index = 0;
  for(auto& elem: attendanceJSON.GetArray()){
	count_type num_days = 1;
	if(elem.HasMember("num_days")){
		num_days = elem["num_days"].GetInt();
		ATTENDANCE.attendance_new_file_type = true;
	}
	for (count_type day = 0; day < num_days; ++day){

		ATTENDANCE.probabilities.push_back(vector<double>(GLOBAL.NUMBER_OF_OFFICE_TYPES));
		count_type val;
		std::string val_s;

		val = static_cast<count_type>(OfficeType::other);
		val_s = std::to_string(val);
		ATTENDANCE.probabilities[index].at(val) = elem[val_s.c_str()].GetDouble();
		
		val = static_cast<count_type>(OfficeType::sez);
		val_s = std::to_string(val);
		ATTENDANCE.probabilities[index].at(val) = elem[val_s.c_str()].GetDouble();
		
		val = static_cast<count_type>(OfficeType::government);
		val_s = std::to_string(val);
		ATTENDANCE.probabilities[index].at(val) = elem[val_s.c_str()].GetDouble();
		
		val = static_cast<count_type>(OfficeType::it);
		val_s = std::to_string(val);
		ATTENDANCE.probabilities[index].at(val) = elem[val_s.c_str()].GetDouble();
		
		val = static_cast<count_type>(OfficeType::construction);
		val_s = std::to_string(val);
		ATTENDANCE.probabilities[index].at(val) = elem[val_s.c_str()].GetDouble();
		
		val = static_cast<count_type>(OfficeType::hospital);
		val_s = std::to_string(val);
		ATTENDANCE.probabilities[index].at(val) = elem[val_s.c_str()].GetDouble();
		
		++index;
	}
  }
  ATTENDANCE.number_of_entries = index;
  //assert(index == ATTENDANCE.number_of_entries);
}
