//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "interventions.h"
const int UPPER_AGE = 65;
using std::vector;

double kappa_T(const agent& node, double cur_time){
  double val = 0;
  if(!node.infective){
	val = 0;
  }
  else {
	double time_since_infection = cur_time - node.time_of_infection;

	if(time_since_infection < node.incubation_period
	   || time_since_infection> (node.incubation_period
								 + node.asymptomatic_period
								 + node.symptomatic_period)) {
	  // Individual is not yet symptomatic or has been recovered, or has moved to the hospital
	  val = 0;
	} else if(time_since_infection < node.incubation_period + node.asymptomatic_period) {
	  val = 1;
	} else {
	  val = 1.5;
	}
  }
  return val;
}


void get_kappa_no_intervention(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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

void get_kappa_case_isolation(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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

	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * GLOBAL.SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
  }
}

void get_kappa_SC(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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

	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS + SELF_ISOLATION_DAYS) * GLOBAL.SIM_STEPS_PER_DAY)){
	  nodes[count].quarantined = true;
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
	if(nodes[count].workplace_type==WorkplaceType::school){
			nodes[count].kappa_W = 0;
			nodes[count].kappa_W_incoming = 0;
	}
  }
}

void get_kappa_home_quarantine(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count < homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms
		<= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H = 2;
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
  }
}

void get_kappa_lockdown(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
#pragma omp parallel for
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
	  }
	  else{
		nodes[count].kappa_W = 0;
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

void get_kappa_CI_HQ(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
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
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY)
	   && (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS + HOME_QUARANTINE_DAYS) *GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for(count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H = 1; //case isolation reduces household interactions.
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
  }
}

void get_kappa_CI_HQ_65P(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H = 1;
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
  }
}

void get_kappa_LOCKDOWN_fper_CI_HQ_SD_65_PLUS_sper_CI(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P(nodes, homes, workplaces, communities, cur_time);
	}else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_LOCKDOWN_fper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time, double FIRST_PERIOD){
  if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
  }else{
	get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
  }
}

void get_kappa_CI_HQ_65P_SC(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H = 1;
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
	if(nodes[count].workplace_type==WorkplaceType::school){
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	}
  }
}

void get_kappa_CI_HQ_65P_SC_OE(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time){
  for(count_type count = 0; count<homes.size(); ++count){
	//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
	homes[count].quarantined = false;
  }

  for (count_type count = 0; count < nodes.size(); ++count){
	double time_since_symptoms = cur_time
	  - (nodes[count].time_of_infection
		 + nodes[count].incubation_period
		 + nodes[count].asymptomatic_period);
	if((nodes[count].compliant) &&
	   (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*GLOBAL.SIM_STEPS_PER_DAY) &&
	   (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*GLOBAL.SIM_STEPS_PER_DAY)){
	  homes[nodes[count].home].quarantined = true;
	}
  }

#pragma omp parallel for
  for (count_type count = 0; count < nodes.size(); ++count){
	nodes[count].kappa_T = kappa_T(nodes[count], cur_time);
	nodes[count].kappa_H = 1;
	nodes[count].kappa_W = 1;
	nodes[count].kappa_C = 1;
	nodes[count].quarantined = false;
	nodes[count].kappa_H_incoming = 1;
	nodes[count].kappa_W_incoming = 1;
	nodes[count].kappa_C_incoming = 1;

	if(homes[nodes[count].home].quarantined){
	  nodes[count].quarantined = true;
	  nodes[count].kappa_H = 1;
	  nodes[count].kappa_W = 0.25;
	  nodes[count].kappa_C = 0.25;
	  nodes[count].kappa_H_incoming = 1;
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
	if(nodes[count].age>= UPPER_AGE && nodes[count].compliant){
	  nodes[count].kappa_W_incoming = 0.25;
	  nodes[count].kappa_C_incoming = 0.25;
	}
	if(nodes[count].workplace_type==WorkplaceType::office){
		//odd-even rule for workplaces. 50% interactions for workplaces.
		nodes[count].kappa_W = 0.25;
		nodes[count].kappa_W_incoming = 0.25;
	} else {
		//school and colleges are closed
		nodes[count].kappa_W = 0;
		nodes[count].kappa_W_incoming = 0;
	}
  }
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper_SC_tper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD, double THIRD_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_SC(nodes, homes, workplaces, communities, cur_time);
	}else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time, double FIRST_PERIOD, double SECOND_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P_SC(nodes, homes, workplaces, communities, cur_time);
	} else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

void get_kappa_LD_fper_CI_HQ_SD65_SC_OE_sper(vector<agent>& nodes, vector<house>& homes, const vector<workplace>& workplaces, const vector<community>& communities, int cur_time, double FIRST_PERIOD, double OE_SECOND_PERIOD){
	if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_lockdown(nodes, homes, workplaces, communities, cur_time);
	} else if(cur_time < (GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS+FIRST_PERIOD+OE_SECOND_PERIOD)*GLOBAL.SIM_STEPS_PER_DAY){
	  get_kappa_CI_HQ_65P_SC_OE(nodes, homes, workplaces, communities, cur_time);
	} else{
	  get_kappa_case_isolation(nodes, homes, workplaces, communities, cur_time);
	}
}

