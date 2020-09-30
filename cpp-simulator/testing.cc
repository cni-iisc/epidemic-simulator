//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "testing.h"
#include "intervention_primitives.h"
#include <cassert>
using std::vector;


void set_test_request(vector<agent>& nodes, const vector<house>& homes, const vector<workplace>& workplaces, const vector<vector<nbr_cell>>& nbr_cells, const vector<community>& communities, const testing_probability probabilities, const count_type current_time){
	
  for(count_type i=0; i<nodes.size(); ++i){
	double time_since_hospitalised = current_time
                - (nodes[i].time_of_infection
                        + nodes[i].incubation_period
                        + nodes[i].asymptomatic_period
                        + nodes[i].symptomatic_period);
	double time_since_symptomatic = current_time - (nodes[i].time_of_infection
                        + nodes[i].incubation_period
                        + nodes[i].asymptomatic_period);

  	//double time_since_tested = current_time - nodes[i].test_status.tested_epoch;

	//First, decide whether to test the node. Triggers for testing.

	//1) Node just turned symptomatic, 2) node was not tested positive before,  and 3) the coin toss decided to test the node
	if(nodes[i].infection_status == Progression::symptomatic && 
	  time_since_symptomatic > 0 && time_since_symptomatic <= 1 &&
	  bernoulli(probabilities.prob_test_index_symptomatic) &&
	  !nodes[i].test_status.tested_positive){		
		nodes[i].test_status.test_requested = true;
		nodes[i].test_status.node_test_trigger = test_trigger::symptomatic;
		nodes[i].test_status.contact_traced_epoch = current_time; //This is to ensure that if the node's test turns positive, they are also subjected to restrictions.		
	}
	//1) Node just turned symptomatic, 2) node was not tested positive before,  and 3) the coin toss decided to test the node
	else if(nodes[i].infection_status == Progression::hospitalised &&
	  time_since_hospitalised > 0 && time_since_hospitalised <= 1 &&
	  bernoulli(probabilities.prob_test_index_hospitalised) &&
	  !nodes[i].test_status.tested_positive){
		nodes[i].test_status.test_requested = true;
		nodes[i].test_status.node_test_trigger = test_trigger::hospitalised;
		nodes[i].test_status.contact_traced_epoch = current_time; //This is to ensure that if their test turns positive, they are also subjected to contact traced restrictions.
	}
	// Re-test if somebody is recovered
	else if(nodes[i].test_status.state==test_result::positive &&
	  nodes[i].infection_status==Progression::recovered &&
	  bernoulli(probabilities.prob_retest_recovered)){
		nodes[i].test_status.test_requested = true;
		nodes[i].test_status.node_test_trigger = test_trigger::re_test;
	}

	
	// Trigger contact trace from node. Enter only if the node has not yet triggered a contact trace, and if the node tested postive.
	if(!nodes[i].test_status.triggered_contact_trace && nodes[i].test_status.tested_positive){
		nodes[i].test_status.triggered_contact_trace = true; // record that contact tracing was triggered.
		if(nodes[i].test_status.node_test_trigger == test_trigger::symptomatic){
		test_contact_trace_household(i,nodes,homes,probabilities.prob_contact_trace_household_symptomatic,probabilities.prob_test_household_symptomatic_symptomatic,probabilities.prob_test_household_symptomatic_asymptomatic, current_time);
		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::hospitalised){
		test_contact_trace_household(i,nodes,homes,probabilities.prob_contact_trace_household_hospitalised,probabilities.prob_test_household_hospitalised_symptomatic,probabilities.prob_test_household_hospitalised_asymptomatic, current_time);
		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::contact_traced){
			test_contact_trace_household(i,nodes,homes,probabilities.prob_contact_trace_household_positive,probabilities.prob_test_household_positive_symptomatic,probabilities.prob_test_household_positive_asymptomatic, current_time);
		}

		// Test people in smaller workplace network

		if(nodes[i].workplace_type==WorkplaceType::school || nodes[i].workplace_type==WorkplaceType::office){
			if(nodes[i].workplace_type==WorkplaceType::school){
				if(nodes[i].test_status.node_test_trigger == test_trigger::symptomatic){
					test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_class_symptomatic,probabilities.prob_test_school_symptomatic_symptomatic,probabilities.prob_test_school_symptomatic_asymptomatic, current_time);
				}
				else if(nodes[i].test_status.node_test_trigger == test_trigger::hospitalised){
					test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_class_hospitalised,probabilities.prob_test_school_hospitalised_symptomatic,probabilities.prob_test_school_hospitalised_asymptomatic, current_time);
				}
				else if(nodes[i].test_status.node_test_trigger == test_trigger::contact_traced){
					test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_class_positive,probabilities.prob_test_school_positive_symptomatic,probabilities.prob_test_school_positive_asymptomatic, current_time);
				}

			}
			else if(nodes[i].workplace_type==WorkplaceType::office){
				if(nodes[i].test_status.node_test_trigger == test_trigger::symptomatic){
					test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_project_symptomatic,probabilities.prob_test_workplace_symptomatic_symptomatic,probabilities.prob_test_workplace_symptomatic_asymptomatic, current_time);
				}
				else if(nodes[i].test_status.node_test_trigger == test_trigger::hospitalised){
					test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_project_hospitalised,probabilities.prob_test_workplace_hospitalised_symptomatic,probabilities.prob_test_workplace_hospitalised_asymptomatic, current_time);
				}
				else if(nodes[i].test_status.node_test_trigger == test_trigger::contact_traced){
					test_contact_trace_project(i,nodes,workplaces,probabilities.prob_contact_trace_project_positive,probabilities.prob_test_workplace_positive_symptomatic,probabilities.prob_test_workplace_positive_asymptomatic, current_time);
				}
			}		
		}

		// Test people in random community network

		if(nodes[i].test_status.node_test_trigger == test_trigger::symptomatic){
			test_contact_trace_random_community(i,nodes,homes,probabilities.prob_contact_trace_random_community_symptomatic,probabilities.prob_test_random_community_symptomatic_symptomatic,probabilities.prob_test_random_community_symptomatic_asymptomatic, current_time);
		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::hospitalised){
			test_contact_trace_random_community(i,nodes,homes,probabilities.prob_contact_trace_random_community_hospitalised,probabilities.prob_test_random_community_hospitalised_symptomatic,probabilities.prob_test_random_community_hospitalised_asymptomatic, current_time);
		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::contact_traced){
			test_contact_trace_random_community(i,nodes,homes,probabilities.prob_contact_trace_random_community_positive,probabilities.prob_test_random_community_positive_symptomatic,probabilities.prob_test_random_community_positive_asymptomatic, current_time);
		}
		
		#ifndef DISABLE_CONTACT_TRACE_NBR_CELLS
		// Test people in neighbourhood cell

		if(nodes[i].test_status.node_test_trigger == test_trigger::symptomatic){
			test_contact_trace_neighbourhood_cell(i,nodes,homes,nbr_cells,probabilities.prob_contact_trace_neighbourhood_symptomatic,probabilities.prob_test_neighbourhood_symptomatic_symptomatic,probabilities.prob_test_neighbourhood_symptomatic_asymptomatic, current_time);
		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::hospitalised){
			test_contact_trace_neighbourhood_cell(i,nodes,homes,nbr_cells,probabilities.prob_contact_trace_neighbourhood_hospitalised,probabilities.prob_test_neighbourhood_hospitalised_symptomatic,probabilities.prob_test_neighbourhood_hospitalised_asymptomatic, current_time);
		}
		else if(nodes[i].test_status.node_test_trigger == test_trigger::contact_traced){
			test_contact_trace_neighbourhood_cell(i,nodes,homes,nbr_cells,probabilities.prob_contact_trace_neighbourhood_positive,probabilities.prob_test_neighbourhood_positive_symptomatic,probabilities.prob_test_neighbourhood_positive_asymptomatic, current_time);
		}
		#endif
	}
  }
}

// Not used now, to be removed
bool should_be_isolated_node_testing(const agent& node, const int current_time, const int quarantine_days){
  double time_since_tested = current_time - node.test_status.tested_epoch;
 return (node.test_status.state==test_result::positive && (node.infection_status==Progression::exposed || node.infection_status==Progression::infective) &&
   (time_since_tested > 0) &&
   (time_since_tested <= quarantine_days*GLOBAL.SIM_STEPS_PER_DAY));
}


void update_infection_testing(vector<agent>& nodes, vector<house>& houses, count_type current_time){
  for(auto& node: nodes){
	if(node.test_status.state==test_result::positive){
		if(node.infection_status==Progression::symptomatic){
			if(node.severity==1){
				node.disease_label = DiseaseLabel::moderate_symptomatic_tested;
			}
			else{
				node.disease_label = DiseaseLabel::mild_symptomatic_tested;
			}
		}
		else if(node.infection_status==Progression::exposed || node.infection_status==Progression::infective){
			node.disease_label = DiseaseLabel::mild_symptomatic_tested;
		}
		else if(node.infection_status==Progression::hospitalised){
			node.disease_label=DiseaseLabel::severe_symptomatic_tested;
		}
		else if(node.infection_status==Progression::critical){
			node.disease_label=DiseaseLabel::icu;
		}
		else if(node.infection_status==Progression::recovered){
			node.disease_label=DiseaseLabel::recovered;
		}
		else if(node.infection_status==Progression::dead){
			node.disease_label=DiseaseLabel::dead;
		}

	}
	if(node.disease_label==DiseaseLabel::primary_contact || node.disease_label==DiseaseLabel::mild_symptomatic_tested || node.disease_label==DiseaseLabel::moderate_symptomatic_tested){
		if(current_time - node.test_status.contact_traced_epoch <= HOME_QUARANTINE_DAYS*GLOBAL.SIM_STEPS_PER_DAY){
			modify_kappa_case_isolate_node(node);
		}
		else{
				node.disease_label=DiseaseLabel::asymptomatic;
		}
	}
}

}

void set_test_request_fileread(vector<agent>& nodes, const vector<house>& homes,
						 const vector<workplace>& workplaces, const matrix<nbr_cell>& nbr_cells,
						 const vector<community>& communities,						 
						 const vector<testing_probability>& testing_probability_vector, const int cur_time){
  count_type time_threshold = GLOBAL.NUM_DAYS_BEFORE_INTERVENTIONS;
  count_type cur_day = cur_time/GLOBAL.SIM_STEPS_PER_DAY; //get current day. Division to avoid multiplication inside for loop.
  const auto SIZE = testing_probability_vector.size();

  assert(SIZE > 0);
  assert(cur_day >= time_threshold);
  count_type intv_index = 0;

  for (count_type count = 0; count < SIZE - 1; ++count){
	time_threshold += testing_probability_vector[count].num_days;
	if(cur_day >= time_threshold){
	  ++intv_index;
	} else {
	  break;
	}
  }
  set_test_request(nodes, homes, workplaces, nbr_cells, communities, testing_probability_vector[intv_index], cur_time);
}


void test_contact_trace_household(count_type node_index, vector<agent>& nodes, const vector<house>& homes, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	for(auto household_member: homes[nodes[node_index].home].individuals){
		if(bernoulli(probability_contact_trace)){//contact trace a household individual with this probability.
		  nodes[household_member].test_status.contact_traced_epoch = current_time;
		  if(nodes[household_member].disease_label == DiseaseLabel::asymptomatic){
			nodes[household_member].disease_label = DiseaseLabel::primary_contact;
		  }
		  if((current_time - nodes[household_member].test_status.tested_epoch
				> GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL) && 
			!nodes[household_member].test_status.tested_positive){//If the individual was not tested yet.
			if(nodes[household_member].infection_status == Progression::symptomatic &&
			 bernoulli(probability_test_symptomatic)){
			  nodes[household_member].test_status.test_requested = true;
			  nodes[household_member].test_status.node_test_trigger = test_trigger::contact_traced;
			}
			else if((nodes[household_member].infection_status == Progression::susceptible ||
					nodes[household_member].infection_status == Progression::exposed ||
					nodes[household_member].infection_status == Progression::infective ||
					(nodes[household_member].infection_status == Progression::recovered &&
					 !nodes[household_member].entered_hospitalised_state)) && //could remove this check of entered hospital as we are already checking if the node ever tested positive.
					bernoulli(probability_test_asymptomatic)){
			  nodes[household_member].test_status.test_requested = true;
			  nodes[household_member].test_status.node_test_trigger = test_trigger::contact_traced;
			}
		  }
		}
	}
}

void test_contact_trace_project(count_type node_index, vector<agent>& nodes, const vector<workplace>& workplaces, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	for(const auto colleague_index: workplaces[nodes[node_index].workplace].projects[nodes[node_index].workplace_subnetwork].individuals){
		if(bernoulli(probability_contact_trace) && 
		!nodes[colleague_index].test_status.tested_positive){
			nodes[colleague_index].test_status.contact_traced_epoch = current_time;
			if(nodes[colleague_index].disease_label == DiseaseLabel::asymptomatic){
				nodes[colleague_index].disease_label = DiseaseLabel::primary_contact;
			}
			if(current_time - nodes[colleague_index].test_status.tested_epoch
				> GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
				if(nodes[colleague_index].infection_status == Progression::symptomatic &&
				 bernoulli(probability_test_symptomatic)){
					nodes[colleague_index].test_status.test_requested = true;
					nodes[colleague_index].test_status.node_test_trigger= test_trigger::contact_traced;
				}
				else if((nodes[colleague_index].infection_status == Progression::susceptible ||
						nodes[colleague_index].infection_status == Progression::exposed ||
						nodes[colleague_index].infection_status == Progression::infective ||
						(nodes[colleague_index].infection_status == Progression::recovered &&
						!nodes[colleague_index].entered_hospitalised_state)) && 
						bernoulli(probability_test_asymptomatic)){
					nodes[colleague_index].test_status.test_requested = true;
					nodes[colleague_index].test_status.node_test_trigger= test_trigger::contact_traced;
				}
			}
		}
	}
}

void test_contact_trace_random_community(count_type node_index, vector<agent>& nodes, const vector<house>& homes, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	for(count_type k=0; k<homes[nodes[node_index].home].random_households.households.size(); k++){
		if(bernoulli(probability_contact_trace)){//Within random community, we think of household connections, to model say family friends.
		  for(const auto cohabitant_index: homes[homes[nodes[node_index].home].random_households.households[k]].individuals){
			nodes[cohabitant_index].test_status.contact_traced_epoch = current_time;
			if(nodes[cohabitant_index].disease_label == DiseaseLabel::asymptomatic){
			  nodes[cohabitant_index].disease_label = DiseaseLabel::primary_contact;
			}
			if((current_time - nodes[cohabitant_index].test_status.tested_epoch
				> GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL) && 
				(!nodes[cohabitant_index].test_status.tested_positive)){
			  if(nodes[cohabitant_index].infection_status == Progression::symptomatic &&
			   bernoulli(probability_test_symptomatic)){
				nodes[cohabitant_index].test_status.test_requested = true;
				nodes[cohabitant_index].test_status.node_test_trigger = test_trigger::contact_traced;
			  }
			  else if((nodes[cohabitant_index].infection_status == Progression::susceptible ||
					  nodes[cohabitant_index].infection_status == Progression::exposed ||
					  nodes[cohabitant_index].infection_status == Progression::infective ||
					  (nodes[cohabitant_index].infection_status == Progression::recovered &&
					   !nodes[cohabitant_index].entered_hospitalised_state)) &&
					  bernoulli(probability_test_asymptomatic)){
				nodes[cohabitant_index].test_status.test_requested = true;
				nodes[cohabitant_index].test_status.node_test_trigger = test_trigger::contact_traced;
			  }
			}
		  }
		}
	}
}

void test_contact_trace_neighbourhood_cell(count_type node_index, vector<agent>& nodes, const vector<house>& homes, const matrix<nbr_cell> nbr_cells, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time ){
	grid_cell my_grid_cell = homes[nodes[node_index].home].neighbourhood;
	nbr_cell my_nbr_cell = nbr_cells[my_grid_cell.cell_x][my_grid_cell.cell_y];
	count_type my_nbr_size = my_nbr_cell.houses_list.size();
	for(count_type k=0; k<my_nbr_size; k++){
		for(const auto neighbor_index: homes[my_nbr_cell.houses_list[k]].individuals){
			if(bernoulli(probability_contact_trace) && !nodes[neighbor_index].test_status.tested_positive){ //Within a neighbourhood we think of individuals connections rather than household connections.
			//We contact trace the individual only if he was never tested positive and if the contact trace Bernoulli for the individual is one.
				nodes[neighbor_index].test_status.contact_traced_epoch = current_time;
				if(nodes[neighbor_index].disease_label == DiseaseLabel::asymptomatic){
					nodes[neighbor_index].disease_label = DiseaseLabel::primary_contact;
				}
				if(current_time - nodes[neighbor_index].test_status.tested_epoch > GLOBAL.SIM_STEPS_PER_DAY*GLOBAL.MINIMUM_TEST_INTERVAL){
					if(nodes[neighbor_index].infection_status == Progression::symptomatic &&
					 bernoulli(probability_test_symptomatic)){
						nodes[neighbor_index].test_status.test_requested = true;
						nodes[neighbor_index].test_status.node_test_trigger = test_trigger::contact_traced;
					}
					else if((nodes[neighbor_index].infection_status == Progression::susceptible ||
							nodes[neighbor_index].infection_status == Progression::exposed ||
							nodes[neighbor_index].infection_status == Progression::infective ||
							(nodes[neighbor_index].infection_status == Progression::recovered &&
								!nodes[neighbor_index].entered_hospitalised_state)) &&
							bernoulli(probability_test_asymptomatic)){
						nodes[neighbor_index].test_status.test_requested = true;
						nodes[neighbor_index].test_status.node_test_trigger=test_trigger::contact_traced;
					}
				}
			}
		}
	}
}