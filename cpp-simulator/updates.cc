//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>

#include "updates.h"
#include "interventions.h"

using std::cerr;
using std::vector;

bool mask_active(int cur_time){
	int mask_start_date = GLOBAL.MASK_START_DATE;
	int MASK_ON_TIME = mask_start_date * GLOBAL.SIM_STEPS_PER_DAY;
	return (cur_time >= MASK_ON_TIME && GLOBAL.MASK_ACTIVE);
}

double update_individual_lambda_h(const agent& node,int cur_time){
  return (node.infective?1.0:0.0)
	* node.kappa_T
	* node.infectiousness
	* (1 + node.severity)
	* node.kappa_H;
}

double update_individual_lambda_w(const agent& node, int cur_time){
  double mask_factor = 1.0;
  if(mask_active(cur_time) && node.compliant){
	  mask_factor = GLOBAL.MASK_FACTOR;
  }
  return (node.infective?1.0:0.0)
    * (node.attending?1.0:GLOBAL.ATTENDANCE_LEAKAGE)
	* node.kappa_T
	* node.infectiousness
	* (1 + node.severity*(2*node.psi_T-1))
	* node.kappa_W
	* mask_factor;
}

double update_individual_lambda_c(const agent& node, int cur_time){
  double mask_factor = 1.0;
  if(mask_active(cur_time) && node.compliant){
	  mask_factor = GLOBAL.MASK_FACTOR;
  }
  return (node.infective?1.0:0.0)
	* node.kappa_T
	* node.infectiousness
	* node.funct_d_ck
	* (1 + node.severity)
	* node.kappa_C
	* mask_factor;
	// optimised version: return node.lambda_h * node.funct_d_ck;
}


//Returns whether the node was infected or turned symptomatic in this time step
node_update_status update_infection(agent& node, int cur_time){
  int age_index = node.age_index;
  bool transition = false;
  node_update_status update_status;
  //console.log(1-Math.exp(-node['lambda']/SIM_STEPS_PER_DAY))
  ///TODO: Parametrise transition times
  if (node.infection_status==Progression::susceptible){
	//#pragma omp critical
	{
	  transition = bernoulli(1-exp(-node.lambda/GLOBAL.SIM_STEPS_PER_DAY));
	}
	if(transition){
	  node.infection_status = Progression::exposed; //move to exposed state
	  node.time_of_infection = cur_time;
	  node.infective = false;
	  update_status.new_infection = true;
	}
  }
  else if(node.infection_status==Progression::exposed
		  && (double(cur_time) - node.time_of_infection
			  > node.incubation_period)){
	node.infection_status = Progression::infective; //move to infective state
	node.infective = true;
	node.time_became_infective = cur_time;
	update_status.new_infective = true;
  }
  else if(node.infection_status==Progression::infective
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period))){
	//#pragma omp critical
	{
	  transition = bernoulli(GLOBAL.SYMPTOMATIC_FRACTION);
	}
	if(transition){
	  node.infection_status = Progression::symptomatic; //move to symptomatic
	  node.infective = true;
	  update_status.new_symptomatic = true;
	  node.entered_symptomatic_state = true;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::symptomatic
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period))){
	//#pragma omp critical
	{
	  transition = bernoulli(STATE_TRAN[age_index][0]);
	}
	if(transition){
	  node.infection_status = Progression::hospitalised; //move to hospitalisation
	  node.infective = false;
	  update_status.new_hospitalization = true;
	  node.entered_hospitalised_state = true;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::hospitalised
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period
				 + node.hospital_regular_period))){
	//#pragma omp critical
	{
	  transition = bernoulli(STATE_TRAN[age_index][1]);
	}
	if(transition){
	  node.infection_status = Progression::critical; //move to critical care
	  node.infective = false;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered; //move to recovered
	  node.infective = false;
	}
  }
  else if(node.infection_status==Progression::critical
		  && (double(cur_time) - node.time_of_infection
			  > (node.incubation_period
				 + node.asymptomatic_period
				 + node.symptomatic_period
				 + node.hospital_regular_period
				 + node.hospital_critical_period))){
	//#pragma omp critical
	{
	  transition = bernoulli(STATE_TRAN[age_index][2]);
	}
	if(transition){
	  node.infection_status = Progression::dead;//move to dead
	  node.infective = false;
	}
	else {
	  node.state_before_recovery = node.infection_status;
	  node.infection_status = Progression::recovered;//move to recovered
	  node.infective = false;
	}
  }
  node.lambda_h = update_individual_lambda_h(node,cur_time);
  node.lambda_w = update_individual_lambda_w(node,cur_time);
  node.lambda_c = update_individual_lambda_c(node,cur_time);

  return update_status;
}

void update_all_kappa(vector<agent>& nodes, vector<house>& homes, vector<workplace>& workplaces, vector<community>& communities, matrix<nbr_cell>& nbr_cells, vector<intervention_params>& intv_params, int cur_time){
  intervention_params intv_params_local;
  if(cur_time < GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS*GLOBAL.SIM_STEPS_PER_DAY){
    //get_kappa_no_intervention(nodes, homes, workplaces, communities,cur_time);
    get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
  }
  else{
    switch(GLOBAL.INTERVENTION){
    case Intervention::no_intervention:
      //get_kappa_no_intervention(nodes, homes, workplaces, communities, cur_time);
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;
    case Intervention::case_isolation:
      intv_params_local.case_isolation = true;
      //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;
    case Intervention::home_quarantine:
      //get_kappa_home_quarantine(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.home_quarantine = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;
    case Intervention::lockdown:
      //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.lockdown = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;
    case Intervention::case_isolation_and_home_quarantine:
      //get_kappa_CI_HQ(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.case_isolation = true;
      intv_params_local.home_quarantine = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;
    case Intervention::case_isolation_and_home_quarantine_sd_65_plus:
      //get_kappa_CI_HQ_65P(nodes, homes, workplaces, communities, cur_time);
      intv_params_local.case_isolation = true;
      intv_params_local.home_quarantine = true;
      intv_params_local.social_dist_elderly = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;
    case Intervention::lockdown_fper_ci_hq_sd_65_plus_sper_ci:
      get_kappa_LOCKDOWN_fper_CI_HQ_SD_65_PLUS_sper_CI(nodes, homes, workplaces, communities, cur_time,
                                                       GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
      break;
    case Intervention::lockdown_fper:
      get_kappa_LOCKDOWN_fper(nodes, homes, workplaces, communities, cur_time, GLOBAL.FIRST_PERIOD);
      break;
    case Intervention::ld_fper_ci_hq_sd65_sc_sper_sc_tper:
      get_kappa_LD_fper_CI_HQ_SD65_SC_sper_SC_tper(nodes, homes, workplaces, communities, cur_time,
                                                   GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD, GLOBAL.THIRD_PERIOD);
      break;
    case Intervention::ld_fper_ci_hq_sd65_sc_sper:
      get_kappa_LD_fper_CI_HQ_SD65_SC_sper(nodes, homes, workplaces, communities, cur_time,
                                           GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
      break;
    case Intervention::ld_fper_ci_hq_sd65_sc_oe_sper:
      get_kappa_LD_fper_CI_HQ_SD65_SC_OE_sper(nodes, homes, workplaces, communities, cur_time,
                                              GLOBAL.FIRST_PERIOD, GLOBAL.OE_SECOND_PERIOD);
	  break;
    case Intervention::intv_fper_intv_sper_intv_tper:
      get_kappa_intv_fper_intv_sper_intv_tper(nodes, homes, workplaces, communities, cur_time,
                                                   GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD, GLOBAL.THIRD_PERIOD);
      break;
    case Intervention::intv_NYC:
      get_kappa_NYC(nodes, homes, workplaces, communities, cur_time);
      break;
    case Intervention::intv_Mum:
	  get_kappa_Mumbai_alternative_version(nodes, homes, workplaces, communities, nbr_cells, cur_time,
                                                   GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
      break;
	case Intervention::intv_Mum_cyclic:
      get_kappa_Mumbai_cyclic(nodes, homes, workplaces, communities, nbr_cells, cur_time,
							  GLOBAL.FIRST_PERIOD, GLOBAL.SECOND_PERIOD);
	  break;
    case Intervention::intv_nbr_containment:
      get_kappa_containment(nodes, homes, workplaces, communities, nbr_cells, cur_time, GLOBAL.FIRST_PERIOD, Intervention::intv_nbr_containment);
      break;
    case Intervention::intv_ward_containment:
      get_kappa_containment(nodes, homes, workplaces, communities, nbr_cells, cur_time, GLOBAL.FIRST_PERIOD, Intervention::intv_ward_containment);
      break;
    case Intervention::intv_file_read:
      get_kappa_file_read(nodes, homes, workplaces, communities, nbr_cells, intv_params, cur_time);
      break;
    default:
      //get_kappa_no_intervention(nodes, homes, workplaces, communities, cur_time);
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_local);
      break;
    }
  }
}

double updated_lambda_w_age_independent(const vector<agent>& nodes, const workplace& workplace){
  double sum_value = 0;
  vector<double> lambda_age_group(GLOBAL.NUM_AGE_GROUPS);
  for (count_type i=0; i < workplace.individuals.size(); ++i){
	sum_value += nodes[workplace.individuals[i]].lambda_w;
  }
  return workplace.scale*sum_value;
}

double updated_lambda_h_age_independent(const vector<agent>& nodes, const house& home){
  double sum_value = 0;
  for (count_type i=0; i<home.individuals.size(); ++i){
	sum_value += nodes[home.individuals[i]].lambda_h;
  }
  return  home.scale*sum_value;
}

vector<double> updated_lambda_h_age_dependent(const vector<agent>& nodes, const house& home, const matrix<double>& home_tx_u, const vector<double>& home_tx_sigma, const matrix<double>& home_tx_vT){
  auto size = home_tx_u.size();

  vector<double> age_component(GLOBAL.NUM_AGE_GROUPS, 0.0);
  vector<double> lambda_age_group(GLOBAL.NUM_AGE_GROUPS, 0.0);
  vector<double> V_tx(GLOBAL.SIGNIFICANT_EIGEN_VALUES, 0.0);

  for (count_type i=0; i<home.individuals.size(); ++i){
      int ind_age_group = nodes[home.individuals[i]].age_group;
      age_component[ind_age_group] += nodes[home.individuals[i]].lambda_h;
  }

  for (count_type eigen_count=0; eigen_count<GLOBAL.SIGNIFICANT_EIGEN_VALUES; ++eigen_count){
    for(count_type count=0; count<size; ++count){
      V_tx[eigen_count] += home_tx_vT[eigen_count][count]
                           * age_component[count];
    }
  }

  for (count_type count=0; count<GLOBAL.NUM_AGE_GROUPS; ++count){
    for (count_type eigen_count=0; eigen_count<GLOBAL.SIGNIFICANT_EIGEN_VALUES; ++eigen_count){
      lambda_age_group[count] += home_tx_u[count][eigen_count]
                          * home_tx_sigma[eigen_count]
                          * V_tx[eigen_count];
    }
	lambda_age_group[count] *= home.scale;
  }
 return lambda_age_group;

}

vector<double> updated_lambda_w_age_dependent(const vector<agent>& nodes, const workplace& workplace, const matrix<double>& workplace_tx_u, const vector<double>& workplace_tx_sigma, const matrix<double>& workplace_tx_vT){

    auto size = workplace_tx_u.size();

    vector<double> age_component(GLOBAL.NUM_AGE_GROUPS, 0.0);
    vector<double> lambda_age_group(GLOBAL.NUM_AGE_GROUPS, 0.0);
    vector<double> V_tx(GLOBAL.SIGNIFICANT_EIGEN_VALUES, 0.0);
    for (count_type i=0; i<workplace.individuals.size(); ++i){
        int ind_age_group = nodes[workplace.individuals[i]].age_group;
        age_component[ind_age_group] += nodes[workplace.individuals[i]].lambda_w;
    }

    for (count_type eigen_count=0; eigen_count<GLOBAL.SIGNIFICANT_EIGEN_VALUES; ++eigen_count){
      for(count_type count=0; count<size; ++count){
        V_tx[eigen_count] += workplace_tx_vT[eigen_count][count]
                             * age_component[count];
      }
    }

    for (count_type count=0; count<GLOBAL.NUM_AGE_GROUPS; ++count){
      for (count_type eigen_count=0; eigen_count<GLOBAL.SIGNIFICANT_EIGEN_VALUES; ++eigen_count){
        lambda_age_group[count] += workplace_tx_u[count][eigen_count]
                            * workplace_tx_sigma[eigen_count]
                            * V_tx[eigen_count];
      }
	  lambda_age_group[count] *=  workplace.scale;
    }
    return lambda_age_group;
}

vector<double> updated_lambda_c_local_age_dependent(const vector<agent>& nodes, const community& community, const matrix<double>& community_tx_u, const vector<double>& community_tx_sigma, const matrix<double>& community_tx_vT){

  auto size = community_tx_u.size();

  vector<double> age_component(GLOBAL.NUM_AGE_GROUPS, 0.0);
  vector<double> lambda_age_group(GLOBAL.NUM_AGE_GROUPS, 0.0);
  vector<double> V_tx(GLOBAL.SIGNIFICANT_EIGEN_VALUES, 0.0);

  for (count_type i=0; i<community.individuals.size(); ++i){
      int ind_age_group = nodes[community.individuals[i]].age_group;
      age_component[ind_age_group] += nodes[community.individuals[i]].lambda_h;
  }

  for (count_type eigen_count=0; eigen_count<GLOBAL.SIGNIFICANT_EIGEN_VALUES; ++eigen_count){
    for(count_type count=0; count<size; ++count){
      V_tx[eigen_count] += community_tx_vT[eigen_count][count]
                           * age_component[count];
    }
  }

  for (count_type count=0; count<GLOBAL.NUM_AGE_GROUPS; ++count){
    for (count_type eigen_count=0; eigen_count<GLOBAL.SIGNIFICANT_EIGEN_VALUES; ++eigen_count){
      lambda_age_group[count] += community_tx_u[count][eigen_count]
                          * community_tx_sigma[eigen_count]
                          * V_tx[eigen_count];
    }
	lambda_age_group[count] *=  community.scale;
  }
 return lambda_age_group;
}

double updated_travel_fraction(const vector<agent>& nodes, const int cur_time){
  double infected_distance = 0, total_distance = 0;
  count_type actual_travellers = 0, usual_travellers = 0;

  const auto SIZE = nodes.size();
  const auto MASK_FACTOR = GLOBAL.MASK_FACTOR;
#pragma omp parallel for default(none) shared(nodes) \
  reduction (+: usual_travellers, actual_travellers,  \
			 infected_distance, total_distance)
  for(count_type i = 0; i < SIZE; ++i){
	if(nodes[i].has_to_travel){
	  ++usual_travellers;
	}
	if(nodes[i].travels()){
	  double mask_factor = 1.0;
	  if(mask_active(cur_time) && nodes[i].compliant){
		mask_factor = MASK_FACTOR;
	  }
	  ++actual_travellers;
	  total_distance += nodes[i].commute_distance;
	  if(nodes[i].infective){
		infected_distance += nodes[i].commute_distance * mask_factor;
	  }
	}
  }
  if(total_distance == 0 || usual_travellers == 0){
	  return 0;
  } else{
	  return (infected_distance/total_distance)
	* double(actual_travellers)/double(usual_travellers);
  }
  
}


void update_lambdas(agent&node, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const double travel_fraction, const int cur_time){
  node.lambda_incoming.set_zero();
  //Contributions from home, workplace, community, and travel
  if (GLOBAL.USE_AGE_DEPENDENT_MIXING){
    node.lambda_incoming.home = node.kappa_H_incoming
	  * homes[node.home].age_dependent_mixing[node.age_group]
	  * node.hd_area_factor;

    if(node.workplace != WORKPLACE_HOME) {
	  node.lambda_incoming.work = (node.attending?1.0:GLOBAL.ATTENDANCE_LEAKAGE)*node.kappa_W_incoming
		* workplaces[node.workplace].age_dependent_mixing[node.age_group];
    }
    
  }
  else {
	//No null check for home as every agent has a home
	node.lambda_incoming.home = node.kappa_H_incoming
	  * homes[node.home].age_independent_mixing
	  * node.hd_area_factor;
	//If the agent lives in a high population density area, eg, a slum

	//FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	if(node.workplace != WORKPLACE_HOME) {
	  node.lambda_incoming.work = (node.attending?1.0:GLOBAL.ATTENDANCE_LEAKAGE)*node.kappa_W_incoming
		* workplaces[node.workplace].age_independent_mixing;
	  //FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	}
  }
  // No null check for community as every node has a community.
  //
  // For all communities add the community lambda with a distance
  // related scaling factor
  node.lambda_incoming.community = node.kappa_C_incoming
	* node.zeta_a
	* node.funct_d_ck
	* communities[node.community].lambda_community_global
	* node.hd_area_factor
	* pow(communities[node.community].individuals.size(),
		  node.hd_area_exponent);
  //If the agent lives in a high population density area, eg, a slum

  //Travel only happens at "odd" times, twice a day
  if((cur_time % 2) && node.travels()){
	node.lambda_incoming.travel = GLOBAL.BETA_TRAVEL
	  * node.commute_distance
	  * travel_fraction;
  }

  if(mask_active(cur_time) && node.compliant){
	node.lambda_incoming.work *= GLOBAL.MASK_FACTOR;
	node.lambda_incoming.community *= GLOBAL.MASK_FACTOR;
	node.lambda_incoming.travel *= GLOBAL.MASK_FACTOR;
  }

  node.lambda = node.lambda_incoming.sum();

}


double updated_lambda_c_local(const vector<agent>& nodes, const community& community){
  double sum_value = 0;
  const auto SIZE = community.individuals.size();

#pragma omp parallel for default(none) shared(nodes, community) reduction (+: sum_value)
  for(count_type i = 0; i < SIZE; ++i){
	sum_value += nodes[community.individuals[i]].lambda_c;
  }
  //std::fill(lambda_age_group.begin(), lambda_age_group.end(), community.scale*sum_value*community.w_c);
  //return lambda_age_group;
  return community.scale*sum_value*community.w_c;
}

void update_lambda_c_global(vector<community>& communities,
							const matrix<double>& community_distance_fk_matrix){
  const auto SIZE = communities.size();
  for (count_type c1 = 0; c1 < SIZE; ++c1){
	double num = 0;
	double denom = 0;

	for (count_type c2 = 0; c2 < SIZE; ++c2){
	  double fk_val = community_distance_fk_matrix[c1][c2];
	  num += fk_val * communities[c2].lambda_community;
	  denom += fk_val;
	}
	if(denom==0){		
		communities[c1].lambda_community_global = 0;
	} else{		
		communities[c1].lambda_community_global = communities[c1].w_c*num/denom;
	}
	
  }
}


casualty_stats get_infected_community(const vector<agent>& nodes, const community& community){
  count_type affected = 0;
  count_type hd_area_affected = 0;
  count_type susceptible = 0;
  count_type hd_area_susceptible = 0;
  count_type exposed = 0;
  count_type hd_area_exposed = 0;
  count_type infective = 0;
  count_type hd_area_infective = 0;
  count_type symptomatic = 0;
  count_type hd_area_symptomatic = 0;
  count_type hospitalised = 0;
  count_type hd_area_hospitalised = 0;
  count_type critical = 0;
  count_type hd_area_critical = 0;
  count_type dead = 0;
  count_type hd_area_dead = 0;
  count_type recovered = 0;
  count_type hd_area_recovered = 0;
  count_type recovered_from_infective = 0;
  count_type recovered_from_symptomatic = 0;
  count_type recovered_from_hospitalised = 0;
  count_type recovered_from_critical = 0;
  count_type hd_area_recovered_from_infective = 0;
  count_type hd_area_recovered_from_symptomatic = 0;
  count_type hd_area_recovered_from_hospitalised = 0;
  count_type hd_area_recovered_from_critical = 0;

  count_type errors = 0;
  
  const auto SIZE = community.individuals.size(); 

#pragma omp parallel for default(none) shared(nodes, community)			\
  reduction(+: errors,													\
			susceptible, hd_area_susceptible,							\
			exposed, hd_area_exposed,									\
			infective, hd_area_infective,								\
			symptomatic, hd_area_symptomatic,							\
			hospitalised, hd_area_hospitalised,							\
			critical, hd_area_critical,									\
			dead, hd_area_dead,											\
			recovered, hd_area_recovered,								\
			recovered_from_infective, recovered_from_symptomatic,		\
			recovered_from_hospitalised, recovered_from_critical,		\
			hd_area_recovered_from_infective,							\
			hd_area_recovered_from_symptomatic,							\
			hd_area_recovered_from_hospitalised,						\
			hd_area_recovered_from_critical)
  for (count_type i=0; i<SIZE; ++i){
	bool hd_area_resident = nodes[community.individuals[i]].hd_area_resident;
	auto infection_status = nodes[community.individuals[i]].infection_status;
	if (infection_status == Progression::susceptible){
	  susceptible += 1;
	  if(hd_area_resident){
		hd_area_susceptible += 1;
	  }
	}
	if (infection_status == Progression::exposed) {
	  exposed +=1;
	  if(hd_area_resident){
        hd_area_exposed += 1;
      }
	}
	if (infection_status == Progression::infective) {
	  infective +=1;
	  if(hd_area_resident){
        hd_area_infective += 1;
      }
	}
	if (infection_status == Progression::symptomatic) {
	  symptomatic += 1;
      if(hd_area_resident){
        hd_area_symptomatic += 1;
      }
	}
	if (infection_status == Progression::recovered) {
	  recovered += 1;
	  if(hd_area_resident){
        hd_area_recovered += 1;
      }
	  auto state_before_recovery
		= nodes[community.individuals[i]].state_before_recovery;
	  switch(state_before_recovery){
	  case Progression::infective:
		recovered_from_infective += 1;
		if(hd_area_resident){
		  hd_area_recovered_from_infective += 1;
		}
		break;
	  case Progression::symptomatic:
		recovered_from_symptomatic += 1;
		if(hd_area_resident){
		  hd_area_recovered_from_symptomatic += 1;
		}
		break;
	  case Progression::hospitalised:
		recovered_from_hospitalised += 1;
		if(hd_area_resident){
		  hd_area_recovered_from_hospitalised += 1;
		}
		break;
	  case Progression::critical:
		recovered_from_critical += 1;
		if(hd_area_resident){
		  hd_area_recovered_from_critical += 1;
		}
		break;
	  default:
		errors += 1; //errors state_before_recovery
		break;
	  }
	}
	if (infection_status == Progression::hospitalised) {
	  hospitalised += 1;
      if(hd_area_resident){
        hd_area_hospitalised += 1;
      }
	}
	if (infection_status == Progression::critical) {
	  critical += 1;
      if(hd_area_resident){
        hd_area_critical += 1;
      }
	}
	if (infection_status == Progression::dead) {
	  dead += 1;
	  if(hd_area_resident){
        hd_area_dead += 1;
      }
	}
  }
  if(errors){
	cerr << "erroneous state_before_recovery found\n";
	assert(false);
  }
  
  affected = exposed + infective + symptomatic
	+ hospitalised + critical
	+ recovered + dead;

  hd_area_affected = hd_area_exposed + hd_area_infective + hd_area_symptomatic
	+ hd_area_hospitalised + hd_area_critical
	+ hd_area_recovered + hd_area_dead;

  
  casualty_stats stat;
  stat.affected = affected;
  stat.hd_area_affected = hd_area_affected;
  stat.susceptible = susceptible;
  stat.hd_area_susceptible = hd_area_susceptible;
  stat.exposed = exposed;
  stat.hd_area_exposed = hd_area_exposed;
  stat.infective = infective;
  stat.hd_area_infective = hd_area_infective;
  stat.symptomatic = symptomatic;
  stat.hd_area_symptomatic = hd_area_symptomatic;
  stat.hospitalised = hospitalised;
  stat.hd_area_hospitalised = hd_area_hospitalised;
  stat.critical = critical;
  stat.hd_area_critical = hd_area_critical;
  stat.dead = dead;
  stat.hd_area_dead = hd_area_dead;
  stat.recovered = recovered;
  stat.hd_area_recovered = hd_area_recovered;
  stat.recovered_from_infective = recovered_from_infective;
  stat.recovered_from_symptomatic = recovered_from_symptomatic;
  stat.recovered_from_hospitalised = recovered_from_hospitalised;
  stat.recovered_from_critical = recovered_from_critical;
  stat.hd_area_recovered_from_infective = hd_area_recovered_from_infective;
  stat.hd_area_recovered_from_symptomatic = hd_area_recovered_from_symptomatic;
  stat.hd_area_recovered_from_hospitalised = hd_area_recovered_from_hospitalised;
  stat.hd_area_recovered_from_critical = hd_area_recovered_from_critical;

  return stat;
  // Populate it afterwards...
}
