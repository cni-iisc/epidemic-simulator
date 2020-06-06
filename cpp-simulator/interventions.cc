//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include <cassert>

#include "models.h"
#include "intervention_primitives.h"
#include "interventions.h"


using std::vector;
using std::min;

void get_kappa_no_intervention(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
#pragma omp parallel for default(none) shared(nodes)
  for (count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;
  }
}

void get_kappa_case_isolation(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  const auto SIM_STEPS_PER_DAY = GLOBAL.SIM_STEPS_PER_DAY;
#pragma omp parallel for default(none) shared(nodes)
  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if((nodes[count].compliant && nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_SC(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  const auto SIM_STEPS_PER_DAY = GLOBAL.SIM_STEPS_PER_DAY;
#pragma omp parallel for default(none) shared(nodes)
  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if((nodes[count].compliant && nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS * SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
	if(nodes[count].workplace_type==WorkplaceType::school){
			nodes[count].kappa_W = 0;
			nodes[count].kappa_W_incoming = 0;
	}
  }
}

void get_kappa_home_quarantine(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count < homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant && nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_lockdown(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time){
#pragma omp parallel for default(none) shared(nodes)
  for(count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	if(nodes[count].compliant){
	  nodes[count].kappa_H = 2;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	  if(nodes[count].workplace_type==WorkplaceType::office){
		nodes[count].kappa_W = 0.25;
		nodes[count].kappa_W_incoming = 0.25;
	  }
	  else{
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	  }
	}
	else{ //non-compliant
	  nodes[count].kappa_H = 1.25;
	  nodes[count].kappa_C = 1;
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_C_incoming = 1;
	  if(nodes[count].workplace_type==WorkplaceType::office){
		nodes[count].kappa_W = 0.25;
		nodes[count].kappa_W_incoming = 0.25;
	  }
	  else{
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	  }
	}
  }
}

void get_kappa_CI_HQ(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be
	//updated depending on the household individuals.  Same as HQ.
	homes[count].quarantined = false;
  }

  for(count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant && nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY)
	   && (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + HOME_QUARANTINE_DAYS) *GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for(count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_CI_HQ_65P(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant && nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void get_kappa_LOCKDOWN_fper_CI_HQ_SD_65_PLUS_sper_CI(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	intervention_params intv_params;
	matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  intv_params.lockdown = true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine=true;
	  intv_params.social_dist_elderly=true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation=true;
      get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_LOCKDOWN_fper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time, double FIRST_PERIOD){
  intervention_params intv_params;
  matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
  if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	//get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	intv_params.lockdown = true;
	get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
  }else{
	//get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	intv_params.case_isolation = true;
	get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
  }
}

void get_kappa_CI_HQ_65P_SC(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant && nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;

	}
	if(nodes[count].workplace_type==WorkplaceType::school){
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	}
  }
}

void get_kappa_CI_HQ_65P_SC_OE(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, const int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant && nodes[count].entered_symptomatic_state) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for default(none) shared(nodes, homes)
  for (count_type count = 0; count < nodes.size(); ++count){
	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
	
	if(nodes[count].workplace_type==WorkplaceType::office){
		//odd-even rule for workplaces. 50% interactions for workplaces.
		nodes[count].kappa_W = 0.5;
		nodes[count].kappa_W_incoming = 0.5;
	} else {
		//school and colleges are closed
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	}

	//homes SHOULD NOT BE MODIFIED IN THIS LOOP, ONLY READ
	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
      nodes[count].kappa_H = 0.75;
	  nodes[count].kappa_W = 0;
	  nodes[count].kappa_C = 0.1;
      nodes[count].kappa_H_incoming = 0.75;
	  nodes[count].kappa_W_incoming = 0;
	  nodes[count].kappa_C_incoming = 0.1;
	}
  }
}

void reset_community_containment(vector<community>& communities){
	for(count_type count = 0; count<communities.size(); ++count){
		//reset all wards as non-quarantined. The status will be updated depending on the household individuals.
		communities[count].quarantined = false;
	}
}

void mark_communities_for_containment(const vector<agent>& nodes, vector<community>& communities,const int cur_time){
	vector<count_type> num_ward_hospitalised(communities.size(),0);
	for (count_type count = 0; count < nodes.size(); ++count){
		double time_since_hospitalised = cur_time
		- (nodes[count].time_of_infection
			+ nodes[count].incubation_period
			+ nodes[count].asymptomatic_period
			+ nodes[count].symptomatic_period);
		if(((nodes[count].entered_hospitalised_state) &&
		(time_since_hospitalised <= (HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)) ){
			++num_ward_hospitalised[nodes[count].community];
		}
	}
	for (count_type count = 0; count < communities.size(); ++count){
		if(num_ward_hospitalised[count] > GLOBAL.WARD_CONTAINMENT_THRESHOLD){
			communities[count].quarantined = true;
		}
	}
}

void get_kappa_custom_modular(std::vector<agent>& nodes, std::vector<house>& homes,
							  const std::vector<workplace>& workplaces, std::vector<community>& communities,
							  const matrix<nbr_cell>& nbr_cells,
							  const int cur_time, const intervention_params intv_params){
  if(intv_params.trains_active){
    GLOBAL.TRAINS_RUNNING = true;
    GLOBAL.FRACTION_FORCED_TO_TAKE_TRAIN = intv_params.fraction_forced_to_take_train;
  }

  if(intv_params.home_quarantine || intv_params.neighbourhood_containment){
    reset_home_quarantines(homes);
  }

  if(intv_params.home_quarantine ){
    mark_homes_for_quarantine(nodes, homes, cur_time);
    //Don't isolate them yet; have to assign base kappas first.
    //These members will be isolated at the end.
  }

  if(intv_params.neighbourhood_containment){
	mark_neighbourhood_homes_for_quarantine(nodes, homes, nbr_cells, cur_time);
  }

  if(intv_params.ward_containment){
	reset_community_containment(communities);
	mark_communities_for_containment(nodes, communities,cur_time);
  }

#pragma omp parallel for default(none) shared(nodes, homes, communities)
  for (count_type count = 0; count < nodes.size(); ++count){
    //choose base kappas
    if(intv_params.lockdown){
      set_kappa_lockdown_node(nodes[count], cur_time);
    }else{
      set_kappa_base_node(nodes[count], intv_params.community_factor, cur_time);
    }

    //modifiers begin
    if(intv_params.social_dist_elderly){
      modify_kappa_SDE_node(nodes[count]);
    }
    if(intv_params.workplace_odd_even){
	  //This is only for the old attendance implementation.  Now odd even should
	  //be implemented in the attendance file.
      modify_kappa_OE_node(nodes[count]);
    }
    if(intv_params.school_closed){
      modify_kappa_SC_node(nodes[count], intv_params.SC_factor);
    }
    if(intv_params.case_isolation){
      if(nodes[count].compliant && should_be_isolated_node(nodes[count], cur_time, SELF_ISOLATION_DAYS)){
        modify_kappa_case_isolate_node(nodes[count]);
      }
    }
	if(homes[nodes[count].home].quarantined
	   && (intv_params.home_quarantine || (intv_params.neighbourhood_containment))){
	    modify_kappa_case_isolate_node(nodes[count]);
	}
	if(nodes[count].compliant
	   && communities[nodes[count].community].quarantined
	   && intv_params.ward_containment){
	  modify_kappa_ward_containment(nodes[count]);
	}
  }
  /*
	if(intv_params.home_quarantine){
	isolate_quarantined_residents(nodes, homes, cur_time);
	}
  */
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper_SC_tper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD){
	intervention_params intv_params;
	matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	intervention_params intv_params;
	matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_OE_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time, double FIRST_PERIOD, double OE_SECOND_PERIOD){
	intervention_params intv_params;
	matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+OE_SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC_OE(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  intv_params.workplace_odd_even = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_intv_fper_intv_sper_intv_tper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD){
	intervention_params intv_params;
	matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_CI_HQ(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_NYC(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, vector<community>& communities, const int cur_time){
	const double FIRST_PERIOD = 3;
	const double SECOND_PERIOD = 1;
	const double THIRD_PERIOD = 3;
	const double FOURTH_PERIOD = 5;
	intervention_params intv_params;
	matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	  intv_params.case_isolation = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
		intervention_params intv_params;
		intv_params.case_isolation = true;
		intv_params.school_closed = true;
		intv_params.SC_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
		intervention_params intv_params;
		intv_params.case_isolation = true;
		intv_params.school_closed = true;
		intv_params.SC_factor = 0.75;
		intv_params.community_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD+FOURTH_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
		intervention_params intv_params;
		intv_params.case_isolation = true;
		intv_params.school_closed = true;
		intv_params.SC_factor = 0;
		intv_params.community_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}else{
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_Mumbai_cyclic(vector<agent>& nodes, vector<house>& homes,
							 const vector<workplace>& workplaces, vector<community>& communities,
							 const matrix<nbr_cell>& nbr_cells,
							 const int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
  auto LOCKDOWN_PERIOD = FIRST_PERIOD + SECOND_PERIOD;
  double USUAL_COMPLIANCE_PROBABILITY = 0.6;
  double HD_AREA_COMPLIANCE_PROBABILITY = 0.5;
  if(cur_time
	 < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS + LOCKDOWN_PERIOD)
	 *GLOBAL.SIM_STEPS_PER_DAY){
	set_compliance(nodes, homes,
				   USUAL_COMPLIANCE_PROBABILITY,
				   HD_AREA_COMPLIANCE_PROBABILITY);
	get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	//Update global travel parameters
	GLOBAL.TRAINS_RUNNING = false;
	GLOBAL.FRACTION_FORCED_TO_TAKE_TRAIN = 0.0;
  } else {
	set_compliance(nodes, homes,
				   USUAL_COMPLIANCE_PROBABILITY,
				   HD_AREA_COMPLIANCE_PROBABILITY);
	//get_kappa_custom(nodes, homes, workplaces, communities, cur_time, true, true, false, true, true, false, 0, 0.75);
	{
	  intervention_params intv;
	  intv.case_isolation = true;
	  intv.home_quarantine = true;
	  intv.social_dist_elderly = true;
	  intv.school_closed = true;
	  intv.community_factor = 0.75;
	  //All others are default values
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv);
	}
	//Update global travel parameters
	GLOBAL.CYCLIC_POLICY_ENABLED = true;
	GLOBAL.NUMBER_OF_CYCLIC_CLASSES = 3;
	GLOBAL.PERIOD_OF_ATTENDANCE_CYCLE = 5;
	GLOBAL.CYCLIC_POLICY_START_DAY
	  = GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS + LOCKDOWN_PERIOD;

	GLOBAL.TRAINS_RUNNING = true;
	GLOBAL.FRACTION_FORCED_TO_TAKE_TRAIN = 1.0;
  }
}

//The version below is an older version, based on a different generic implementation
void get_kappa_Mumbai_alternative_version(vector<agent>& nodes, vector<house>& homes,
										  const vector<workplace>& workplaces, vector<community>& communities,
										  const matrix<nbr_cell>& nbr_cells,
										  int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	intervention_params intv_params;
	//matrix<nbr_cell> nbr_cells; //dummy variable  just to enable get_kappa_custom_modular function call.
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  set_compliance(nodes, homes, 0.8, 0.8); //compliance hard coded to 0.8 post lockdown.
	  //get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
	  set_compliance(nodes, homes, 0.8, 0.8); //compliance hard coded to 0.8 post lockdown.
	  intervention_params intv_params;
	  intv_params.case_isolation = true;
	  intv_params.home_quarantine = true;
	  intv_params.social_dist_elderly = true;
	  intv_params.school_closed = true;
	  intv_params.SC_factor = 0;
	  intv_params.community_factor = 0.75;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	}
}

void get_kappa_containment(vector<agent>& nodes, vector<house>& homes,
						   const vector<workplace>& workplaces, vector<community>& communities,
						   const matrix<nbr_cell>& nbr_cells,
						   int cur_time, double FIRST_PERIOD, Intervention intv){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  intervention_params intv_params;
	  intv_params.lockdown = true;
	  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
	} else{
		if(intv == Intervention::intv_nbr_containment){
			intervention_params intv_params;
	  		intv_params.case_isolation = true;
	  		intv_params.home_quarantine = true;
	  		intv_params.social_dist_elderly = true;
	  		intv_params.school_closed = true;
	  		intv_params.SC_factor = 0;
	  		intv_params.community_factor = 1;
			intv_params.neighbourhood_containment = GLOBAL.ENABLE_CONTAINMENT;
			get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
		} else{
			intervention_params intv_params;
	  		intv_params.case_isolation = true;
	  		intv_params.home_quarantine = true;
	  		intv_params.social_dist_elderly = true;
	  		intv_params.school_closed = true;
	  		intv_params.SC_factor = 0;
	  		intv_params.community_factor = 1;
			intv_params.ward_containment = GLOBAL.ENABLE_CONTAINMENT;
			get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params);
		}
	}	
}

void get_kappa_file_read(vector<agent>& nodes, vector<house>& homes,
						 const vector<workplace>& workplaces, vector<community>& communities,
						 const matrix<nbr_cell>& nbr_cells,
						 const vector<intervention_params>& intv_params_vector, int cur_time){
  count_type time_threshold = GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS;
  count_type cur_day = cur_time/GLOBAL.SIM_STEPS_PER_DAY; //get current day. Division to avoid multiplication inside for loop.
  const auto SIZE = intv_params_vector.size();

  assert(SIZE > 0);
  assert(cur_day >= time_threshold);
  count_type intv_index = 0;

  for (count_type count = 0; count < SIZE - 1; ++count){
	time_threshold += intv_params_vector[count].num_days;
	if(cur_day >= time_threshold){
	  ++intv_index;
	} else {
	  break;
	}
  }

  set_compliance(nodes, homes, intv_params_vector[intv_index].compliance,
				 intv_params_vector[intv_index].compliance_hd);

  get_kappa_custom_modular(nodes, homes, workplaces, communities, nbr_cells, cur_time, intv_params_vector[intv_index]);
}
