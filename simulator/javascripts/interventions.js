//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0

const UPPER_AGE = 65;

function get_kappa_no_intervention(nodes, homes, workplaces, communities,cur_time){
	for (var count = 0; count < nodes.length;count ++){
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1;
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;
	}

}

function get_kappa_SC(nodes, homes, workplaces, communities,cur_time){
	for (var count = 0; count < nodes.length;count ++){
		var time_since_symptoms = cur_time - (nodes[count]['time_of_infection']+nodes[count]['incubation_period']+nodes[count]['asymptomatic_period']);
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1; 
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;


		if((nodes[count]['compliant']) && 
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+SELF_ISOLATION_DAYS)*SIM_STEPS_PER_DAY)){  
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_W'] = 0.25;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['kappa_PT'] = 0.25;
			nodes[count]['kappa_W_incoming'] = 0.25; 
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}
		if(nodes[count]['workplace_type']==WTYPE_SCHOOL){
			nodes[count]['kappa_W'] = 0;
			nodes[count]['kappa_PT'] = 0;
			nodes[count]['kappa_W_incoming'] = 0;
			nodes[count]['kappa_PT_incoming'] = 0;
		}
	}

}

function get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time){
	for (var count = 0; count < nodes.length;count ++){
		var time_since_symptoms = cur_time - (nodes[count]['time_of_infection']+nodes[count]['incubation_period']+nodes[count]['asymptomatic_period']);
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1; 
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;


		if((nodes[count]['compliant']) && 
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+SELF_ISOLATION_DAYS)*SIM_STEPS_PER_DAY)){  
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_W'] = 0.25;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['kappa_PT'] = 0.25;
			nodes[count]['kappa_W_incoming'] = 0.25; 
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}
	}
}

function get_kappa_home_quarantine(nodes, homes, workplaces, communities,cur_time){
	
	for(var count = 0; count<homes.length;count++){
		//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
		homes[count]['quarantined'] = false;
	}

	for (var count = 0; count < nodes.length;count++){
		var time_since_symptoms = cur_time - (nodes[count]['time_of_infection']+nodes[count]['incubation_period']+nodes[count]['asymptomatic_period'])
		if((nodes[count]['compliant']) && 
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*SIM_STEPS_PER_DAY)){ 
			homes[nodes[count]['home']]['quarantined'] = true;
		}
	}
	for (var count = 0; count < nodes.length;count++){
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1;
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;

		if(homes[nodes[count]['home']]['quarantined']){
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_H'] = 2;
			nodes[count]['kappa_W'] = 0.25;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['kappa_PT'] = 0.25;
			nodes[count]['kappa_H_incoming'] = 1;
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		} 		
	}
}

function get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time){
	for (var count = 0; count < nodes.length;count ++){
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		if(nodes[count]['compliant']){
			nodes[count]['kappa_H'] = 2;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_H_incoming'] = 1;
			nodes[count]['kappa_C_incoming'] = 0.25;			
			if(nodes[count]['workplace_type']==WTYPE_OFFICE){
				nodes[count]['kappa_W'] = 0.25;
				nodes[count]['kappa_PT'] = 0.25;
				nodes[count]['kappa_W_incoming'] = 0.25;
				nodes[count]['kappa_PT_incoming'] = 0.25;
			}
			else{
				nodes[count]['kappa_W'] = 0;
				nodes[count]['kappa_PT'] = 0;
				nodes[count]['kappa_W_incoming'] = 0;
				nodes[count]['kappa_PT_incoming'] = 0;
			}		
		}
		else{ //non-compliant
			nodes[count]['kappa_H'] = 1.25; //household interactions increases by 25%
			nodes[count]['kappa_C'] = 1; //community interactions remains same
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_H_incoming'] = 1;
			nodes[count]['kappa_C_incoming'] = 1; //community interactions remains same
			if(nodes[count]['workplace_type']==WTYPE_OFFICE){
				nodes[count]['kappa_W'] = 0.25;
				nodes[count]['kappa_W_incoming'] = 0.25;
				nodes[count]['kappa_PT'] = 0.25;
				nodes[count]['kappa_PT_incoming'] = 0.25;
			}
			else{
				//schools and colleges are anyway closed
				nodes[count]['kappa_W'] = 0;
				nodes[count]['kappa_W_incoming'] = 0;
				nodes[count]['kappa_PT'] = 0;
				nodes[count]['kappa_PT_incoming'] = 0;
			}
		}		
	}
}

function get_kappa_CI_HQ(nodes, homes, workplaces, communities,cur_time){
	for(var count = 0; count<homes.length;count++){
		//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
		homes[count]['quarantined'] = false;
	}

	for (var count = 0; count < nodes.length;count++){
		var time_since_symptoms = cur_time - (nodes[count]['time_of_infection']+nodes[count]['incubation_period']+nodes[count]['asymptomatic_period'])
		if((nodes[count]['compliant']) && 
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*SIM_STEPS_PER_DAY)){ 
			homes[nodes[count]['home']]['quarantined'] = true;
		}
	}
	for (var count = 0; count < nodes.length;count++){
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1;
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;

		if(homes[nodes[count]['home']]['quarantined']){
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_H'] = 1; //case isolation reduces infectiousness. Everyother parameter is same as HQ.
			nodes[count]['kappa_W'] = 0.25;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['kappa_PT'] = 0.25;
			nodes[count]['kappa_H_incoming'] = 1;
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		} 		
	}
}

function get_kappa_CI_HQ_70P(nodes, homes, workplaces, communities,cur_time){
	for(var count = 0; count<homes.length;count++){
		//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
		homes[count]['quarantined'] = false;
	}

	for (var count = 0; count < nodes.length;count++){
		var time_since_symptoms = cur_time - (nodes[count]['time_of_infection']+nodes[count]['incubation_period']+nodes[count]['asymptomatic_period'])
		if((nodes[count]['compliant']) && 
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*SIM_STEPS_PER_DAY)){ //The magic number 1 = time to recognise symptoms. 
			homes[nodes[count]['home']]['quarantined'] = true;
		}
	}
	for (var count = 0; count < nodes.length;count++){
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1;
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;

		if(homes[nodes[count]['home']]['quarantined']){
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_H'] = 1;
			nodes[count]['kappa_W'] = 0.25;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['kappa_PT'] = 0.25;
			nodes[count]['kappa_H_incoming'] = 1;
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}
		if(nodes[count]['age']>= UPPER_AGE && nodes[count]['compliant']){
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}			
	}
}

function get_kappa_CI_HQ_70P_SC(nodes, homes, workplaces, communities,cur_time){
	for(var count = 0; count<homes.length;count++){
		//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
		homes[count]['quarantined'] = false;
	}

	for (var count = 0; count < nodes.length;count++){
		var time_since_symptoms = cur_time - (nodes[count]['time_of_infection']+nodes[count]['incubation_period']+nodes[count]['asymptomatic_period'])
		if((nodes[count]['compliant']) && 
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*SIM_STEPS_PER_DAY)){ //The magic number 1 = time to recognise symptoms. 
			homes[nodes[count]['home']]['quarantined'] = true;
		}
	}
	for (var count = 0; count < nodes.length;count++){
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1;
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;

		if(homes[nodes[count]['home']]['quarantined']){
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_H'] = 1;
			nodes[count]['kappa_W'] = 0.25;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['kappa_PT'] = 0.25;
			nodes[count]['kappa_H_incoming'] = 1;
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}
		if(nodes[count]['age']>= UPPER_AGE && nodes[count]['compliant']){
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}
		if(nodes[count]['workplace_type']==WTYPE_SCHOOL){
			nodes[count]['kappa_W'] = 0;
			nodes[count]['kappa_PT'] = 0;
			nodes[count]['kappa_W_incoming'] = 0;
			nodes[count]['kappa_PT_incoming'] = 0;
		}	 		
	}
}

function get_kappa_CI_HQ_70P_SC_OE(nodes, homes, workplaces, communities,cur_time){
	for(var count = 0; count<homes.length;count++){
		//reset all homes as non-quarantined. The status will be updated depending on the household individuals.
		homes[count]['quarantined'] = false;
	}

	for (var count = 0; count < nodes.length;count++){
		var time_since_symptoms = cur_time - (nodes[count]['time_of_infection']+nodes[count]['incubation_period']+nodes[count]['asymptomatic_period'])
		if((nodes[count]['compliant']) && 
		 (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) && 
		 (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*SIM_STEPS_PER_DAY)){ //The magic number 1 = time to recognise symptoms. 
			homes[nodes[count]['home']]['quarantined'] = true;
		}
	}
	for (var count = 0; count < nodes.length;count++){
		nodes[count]['kappa_T'] = kappa_T(nodes[count], cur_time);
		nodes[count]['kappa_H'] = 1;
		nodes[count]['kappa_W'] = 1;
		nodes[count]['kappa_C'] = 1;
		nodes[count]['kappa_PT'] = 1;
		nodes[count]['quarantined'] = false;
		nodes[count]['kappa_H_incoming'] = 1;
		nodes[count]['kappa_W_incoming'] = 1;
		nodes[count]['kappa_C_incoming'] = 1;
		nodes[count]['kappa_PT_incoming'] = 1;

		if(homes[nodes[count]['home']]['quarantined']){
			nodes[count]['quarantined'] = true;
			nodes[count]['kappa_H'] = 1;
			nodes[count]['kappa_W'] = 0.25;
			nodes[count]['kappa_C'] = 0.25;
			nodes[count]['kappa_PT'] = 0.25;
			nodes[count]['kappa_H_incoming'] = 1;
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}
		if(nodes[count]['age']>= UPPER_AGE && nodes[count]['compliant']){
			nodes[count]['kappa_W_incoming'] = 0.25;
			nodes[count]['kappa_C_incoming'] = 0.25;
			nodes[count]['kappa_PT_incoming'] = 0.25;
		}
		if(nodes[count]['workplace_type']==WTYPE_OFFICE){
			//odd-even rule for workplaces. 50% interactions for workplaces.
			nodes[count]['kappa_W'] = 0.5;
			nodes[count]['kappa_PT'] = 0.5;
			nodes[count]['kappa_W_incoming'] = 0.5;
			nodes[count]['kappa_PT_incoming'] = 0.5;
		} else{
			//school and colleges are closed
			nodes[count]['kappa_W'] = 0;
			nodes[count]['kappa_PT'] = 0;
			nodes[count]['kappa_W_incoming'] = 0;
			nodes[count]['kappa_PT_incoming'] = 0;

		} 		
	}
}

function get_kappa_LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI(nodes, homes, workplaces, communities,cur_time){
	const FIRST_PERIOD = 21;
	const SECOND_PERIOD = 21;
	
	if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
	}else if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD + SECOND_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_CI_HQ_70P(nodes, homes, workplaces, communities,cur_time);
	}else{
		get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time);
	}
}

function get_kappa_LOCKDOWN_21(nodes, homes, workplaces, communities,cur_time){
	const FIRST_PERIOD = 21;
	if(cur_time < ( NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD )*SIM_STEPS_PER_DAY){
		get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
	}else{
		get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time);
	}
}

function get_kappa_LD_21_CI_HQ_SD70_SC_21_SC_42(nodes, homes, workplaces, communities,cur_time){
	const FIRST_PERIOD = 21;
	const SECOND_PERIOD = 21;
	const THIRD_PERIOD = 42;
	if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
	} else if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD + SECOND_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_CI_HQ_70P_SC(nodes, homes, workplaces, communities,cur_time);
	} else if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD+SECOND_PERIOD+THIRD_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_SC(nodes, homes, workplaces, communities,cur_time);
	} else{
		get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time);
	}
}

function get_kappa_LD_21_CI_HQ_SD70_SC_21(nodes, homes, workplaces, communities,cur_time){
	const FIRST_PERIOD = 21;
	const SECOND_PERIOD = 21;
	
	if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD) *SIM_STEPS_PER_DAY){
		get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
	} else if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD +SECOND_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_CI_HQ_70P_SC(nodes, homes, workplaces, communities,cur_time);
	} else{
		get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time);
	}
}
function get_kappa_LD_21_CI_HQ_SD70_SC_OE_30(nodes, homes, workplaces, communities,cur_time){
	const FIRST_PERIOD = 21;
	const SECOND_PERIOD = 30;
	
	if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD) *SIM_STEPS_PER_DAY){
		get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
	} else if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD +SECOND_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_CI_HQ_70P_SC_OE(nodes, homes, workplaces, communities,cur_time);
	} else{
		get_kappa_case_isolation(nodes, homes, workplaces, communities,cur_time);
	}
}

function get_kappa_CALIBRATION(nodes, homes, workplaces, communities,cur_time){
	const FIRST_PERIOD = CALIB_NO_INTERVENTION_DURATION;
	const SECOND_PERIOD = CALIB_LOCKDOWN_DURATION;

	if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD) *SIM_STEPS_PER_DAY){
		get_kappa_no_intervention(nodes, homes, workplaces, communities,cur_time);
	}else if(cur_time < (NUM_DAYS_BEFORE_INTERVENTIONS + FIRST_PERIOD +SECOND_PERIOD)*SIM_STEPS_PER_DAY){
		get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
	} else {
		get_kappa_lockdown(nodes, homes, workplaces, communities,cur_time);
	}
}
