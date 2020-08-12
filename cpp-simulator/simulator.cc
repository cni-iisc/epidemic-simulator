//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include "models.h"
#include "initializers.h"
#include "updates.h"
#include "simulator.h"
#include "testing.h"

using std::vector;
using std::string;

#if defined DEBUG || defined TIMING
#include <iostream>
#include <cstdlib>
using std::cerr;
#endif

#ifdef TIMING
#include <chrono>

template <class T>
auto duration(const std::chrono::time_point<T>& start, const std::chrono::time_point<T>& end){
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}
#endif


plot_data_struct run_simulation(){
#ifdef TIMING
  cerr << "simulator: starting JSON read\n";
  auto start_time = std::chrono::high_resolution_clock::now();
#endif
  
  auto homes = init_homes();
  auto workplaces = init_workplaces();
  auto communities = init_community();
  auto nodes = init_nodes();
  auto nbr_cells = init_nbr_cells();
  auto intv_params = init_intervention_params();
  auto testing_protocol_file_read = init_testing_protocol();


  auto community_dist_matrix = compute_community_distances(communities);
  auto community_fk_matrix = compute_community_distances_fkernel(community_dist_matrix);

  svd home_age_matrix,
	school_age_matrix,
	workplace_age_matrix,
	community_age_matrix;

  if(GLOBAL.USE_AGE_DEPENDENT_MIXING){
	home_age_matrix = init_home_age_interaction_matrix();
	school_age_matrix = init_school_age_interaction_matrix();
	workplace_age_matrix = init_workplace_age_interaction_matrix();
	community_age_matrix = init_community_age_interaction_matrix();
  }
  
#ifdef TIMING
    auto end_time = std::chrono::high_resolution_clock::now();
	cerr << "simulator: time for JSON reads (ms): " << duration(start_time, end_time) << "\n";
	start_time = std::chrono::high_resolution_clock::now();
#endif

  assign_individual_home_community(nodes, homes, workplaces, communities);
  //assign_individual_home_community must be called before assign_homes_nbr_cell
  assign_homes_nbr_cell(homes,nbr_cells);
  assign_individual_projects(workplaces, nodes);
  assign_household_community(communities, nodes, homes);
  assign_household_random_community(homes, communities);

  compute_scale_homes(homes);
  compute_scale_workplaces(workplaces);
  compute_scale_communities(nodes, communities);
  compute_scale_random_community(homes, nodes);
  compute_scale_nbr_cells(nodes, nbr_cells, homes);
  double travel_fraction = 0;
  
  //This needs to be done after the initilization.
  plot_data_struct plot_data;
  plot_data.nums =
	{
	 {"num_infected", {}},
	 {"num_exposed", {}},
	 {"num_hospitalised", {}},
	 {"num_symptomatic", {}},
	 {"num_critical", {}},
	 {"num_fatalities", {}},
	 {"num_recovered", {}},
	 {"num_affected", {}},
	 {"num_cases", {}},
	 {"num_cumulative_hospitalizations", {}},
	 {"num_cumulative_infective", {}}
	};
  for(auto& elem: plot_data.nums){
	elem.second.reserve(GLOBAL.NUM_TIMESTEPS);
  }
  plot_data.nums["csvContent"] = {};
  plot_data.nums["csvContent"].reserve(GLOBAL.NUM_TIMESTEPS * GLOBAL.num_communities);

  plot_data.susceptible_lambdas =
	{
	 {"susceptible_lambda", {}},
	 {"susceptible_lambda_H", {}},
	 {"susceptible_lambda_W", {}},
	 {"susceptible_lambda_C", {}},
	 {"susceptible_lambda_T", {}},
	 {"susceptible_lambda_PROJECT", {}},
	 {"susceptible_lambda_NBR_CELL", {}},
	 {"susceptible_lambda_RANDOM_COMMUNITY", {}}
	};

  plot_data.total_lambda_fractions =
	{
	 {"total_fraction_lambda_H", {}},
	 {"total_fraction_lambda_W", {}},
	 {"total_fraction_lambda_C", {}},
	 {"total_fraction_lambda_T", {}},
	 {"total_fraction_lambda_PROJECT", {}},
	 {"total_fraction_lambda_NBR_CELL", {}},
	 {"total_fraction_lambda_RANDOM_COMMUNITY", {}}
	};

  plot_data.mean_lambda_fractions =
	{
	 {"mean_fraction_lambda_H", {}},
	 {"mean_fraction_lambda_W", {}},
	 {"mean_fraction_lambda_C", {}},
	 {"mean_fraction_lambda_T", {}},
	 {"mean_fraction_lambda_PROJECT", {}},
	 {"mean_fraction_lambda_NBR_CELL", {}},
	 {"mean_fraction_lambda_RANDOM_COMMUNITY", {}}
	};

  plot_data.cumulative_mean_lambda_fractions = 
	{
	 {"cumulative_mean_fraction_lambda_H", {}},
	 {"cumulative_mean_fraction_lambda_W", {}},
	 {"cumulative_mean_fraction_lambda_C", {}},
	 {"cumulative_mean_fraction_lambda_T", {}},
	 {"cumulative_mean_fraction_lambda_PROJECT", {}},
	 {"cumulative_mean_fraction_lambda_NBR_CELL", {}},
	 {"cumulative_mean_fraction_lambda_RANDOM_COMMUNITY", {}}
	};

  plot_data.quarantined_stats =
	{
	 {"quarantined_stats", {}},
	 {"curtailment_stats", {}}
	};
	   
  plot_data.disease_label_stats = 
	{
		{"disease_label_stats", {}},
	};
  for(auto& elem: plot_data.susceptible_lambdas){
	elem.second.reserve(GLOBAL.NUM_TIMESTEPS);
  }

#ifdef TIMING
  end_time = std::chrono::high_resolution_clock::now();
  cerr << "simulator: time for setup after JSON reads (ms): " << duration(start_time, end_time) << "\n";

  cerr << "simulator: starting simulation\n";
  start_time = std::chrono::high_resolution_clock::now();
#endif
  lambda_incoming_data total_lambda_fraction_data;
  lambda_incoming_data mean_lambda_fraction_data;
  lambda_incoming_data cumulative_mean_lambda_fraction_data;
  count_type num_cases = 0; // Total number of agents who have progessed to symptomatic so far
  count_type quarantined_num_cases = 0;
  count_type num_cumulative_hospitalizations = 0; //Total number of agents who have had to go to the hospital so far
  count_type num_cumulative_infective = 0; //Total number of people who have progressed to the infective state so far

  count_type num_total_infections = 0;
  //Total number of individuals who have become infected via transmission so far
  //This does not included the initially seeded infections

  std::vector<long double> infections_by_new_infectives(GLOBAL.NUM_TIMESTEPS, 0);
  //For keeping track of infections ascribed to agents that became infective at
  //each time

  const auto NUM_PEOPLE = GLOBAL.num_people;

  for(count_type time_step = 0; time_step < GLOBAL.NUM_TIMESTEPS; ++time_step){
#ifdef DEBUG
	auto start_time_timestep = std::chrono::high_resolution_clock::now();
#endif
	total_lambda_fraction_data.set_zero();
	mean_lambda_fraction_data.set_zero();

	count_type num_new_infections = 0;

    if (time_step % GLOBAL.SIM_STEPS_PER_DAY == 0){
      for(count_type j = 0; j < GLOBAL.num_people; ++j){
        nodes[j].attending =
          bernoulli(std::min(communities[nodes[j].community].w_c,
							 nodes[j].neighborhood_access_factor)
                    * nodes[j].get_attendance_probability(time_step));
		nodes[j].forced_to_take_train = GLOBAL.TRAINS_RUNNING
		  && bernoulli(GLOBAL.FRACTION_FORCED_TO_TAKE_TRAIN);
      }
    }

	//#pragma omp parallel for
	//
	// Since update_infection uses a random number generator with
	// global state, parallelizing this loop is not straightforward.
	// Puttting the generator in a critical section can keep it
	// correct, but slows down the code too much.
	for(count_type j = 0; j < NUM_PEOPLE; ++j){
	  auto node_update_status = update_infection(nodes[j], time_step); 
	  nodes[j].psi_T = psi_T(nodes[j], time_step);

	  if(node_update_status.new_infection){
		++num_new_infections;
		++num_total_infections;
		  
		// update the mean fractions with the new data point normalized_lambda
		  
		{
		  total_lambda_fraction_data += nodes[j].lambda_incoming;
		  auto normalized_lambda = (nodes[j].lambda_incoming / nodes[j].lambda);
		  mean_lambda_fraction_data.mean_update(normalized_lambda, num_new_infections);
		  cumulative_mean_lambda_fraction_data.mean_update(normalized_lambda, num_total_infections);
		}
	  }
	  if(node_update_status.new_symptomatic){
		++num_cases;
	  }
	  if(node_update_status.new_symptomatic && nodes[j].quarantined){
		++quarantined_num_cases;
	  }
	  if(node_update_status.new_hospitalization){
		++num_cumulative_hospitalizations;
	  }
	  if(node_update_status.new_infective){
		++num_cumulative_infective;
	  }
	}

	update_all_kappa(nodes, homes, workplaces, communities, nbr_cells, intv_params, time_step);
	if(GLOBAL.ENABLE_TESTING){
		update_test_status(nodes, time_step);
		update_infection_testing(nodes, homes, time_step);
	    update_test_request(nodes, homes, workplaces, communities, nbr_cells, time_step,testing_protocol_file_read);
	}
	if(GLOBAL.USE_AGE_DEPENDENT_MIXING){
	  for (count_type h = 0; h < GLOBAL.num_homes; ++h){
		updated_lambda_h_age_dependent(nodes, homes[h],
									   home_age_matrix.u,
									   home_age_matrix.sigma,
									   home_age_matrix.vT);
	  }
	  for (count_type w = 0; w < GLOBAL.num_schools + GLOBAL.num_workplaces; ++w){
		if(workplaces[w].workplace_type == WorkplaceType::school){
		  updated_lambda_w_age_dependent(nodes, workplaces[w],
										 school_age_matrix.u,
										 school_age_matrix.sigma,
										 school_age_matrix.vT);
		}
		else{
		  updated_lambda_w_age_dependent(nodes, workplaces[w],
										 workplace_age_matrix.u,
										 workplace_age_matrix.sigma,
										 workplace_age_matrix.vT);
		}
		updated_lambda_project(nodes, workplaces[w]);
	  }
	}
    else{
	  for (count_type h = 0; h < GLOBAL.num_homes; ++h){
		updated_lambda_h_age_independent(nodes, homes[h]);
		//FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	  }
	  
	  for (count_type w = 0; w < GLOBAL.num_schools + GLOBAL.num_workplaces; ++w){
		updated_lambda_w_age_independent(nodes, workplaces[w]);
		updated_lambda_project(nodes, workplaces[w]);
		//FEATURE_PROPOSAL: make the mixing dependent on node.age_group;
	  }
	}

	if(GLOBAL.ENABLE_NEIGHBORHOOD_SOFT_CONTAINMENT){
	  update_grid_cell_statistics(nbr_cells, homes, nodes,
								  GLOBAL.LOCKED_NEIGHBORHOOD_LEAKAGE,
								  GLOBAL.NEIGHBORHOOD_LOCK_THRESHOLD);
	}

	for (count_type c = 0; c < GLOBAL.num_communities; ++c){
	  auto temp_stats = get_infected_community(nodes, communities[c]);
	  //let row = [time_step/SIM_STEPS_PER_DAY,c,temp_stats[0],temp_stats[1],temp_stats[2],temp_stats[3],temp_stats[4]].join(",");
	  plot_data.nums["csvContent"].push_back({time_step, {
		  c,
		  temp_stats.affected,
		  temp_stats.susceptible,
		  temp_stats.exposed,
		  temp_stats.infective,
		  temp_stats.symptomatic,
		  temp_stats.hospitalised,
		  temp_stats.critical,
		  temp_stats.dead,
		  temp_stats.recovered,
		  temp_stats.recovered_from_infective,
		  temp_stats.recovered_from_symptomatic,
		  temp_stats.recovered_from_hospitalised,
		  temp_stats.recovered_from_critical,
		  temp_stats.hd_area_affected,
		  temp_stats.hd_area_susceptible,
		  temp_stats.hd_area_exposed,
          temp_stats.hd_area_infective,
		  temp_stats.hd_area_symptomatic,
          temp_stats.hd_area_hospitalised,
          temp_stats.hd_area_critical,
          temp_stats.hd_area_dead,
		  temp_stats.hd_area_recovered,
		  temp_stats.hd_area_recovered_from_infective,
		  temp_stats.hd_area_recovered_from_symptomatic,
		  temp_stats.hd_area_recovered_from_hospitalised,
		  temp_stats.hd_area_recovered_from_critical
		  }});

	  //Update w_c value for this community, followed by update of lambdas
	  if(communities[c].individuals.size()>0){
		communities[c].w_c = interpolate(1.0, GLOBAL.LOCKED_COMMUNITY_LEAKAGE,
										 double(temp_stats.hospitalised)/double(communities[c].individuals.size()),
										 GLOBAL.COMMUNITY_LOCK_THRESHOLD);
	  } else{
		communities[c].w_c = 1;
	  }

	  updated_lambda_c_local(nodes, communities[c]);
	}

	updated_lambda_c_local_random_community(nodes, communities, homes);
	update_lambda_c_global(communities, community_fk_matrix);
	update_lambda_nbr_cells(nodes, nbr_cells, homes, communities);

	travel_fraction = updated_travel_fraction(nodes,time_step);

	// Update lambdas for the next step
#pragma omp parallel for default(none)									\
  shared(travel_fraction, time_step, homes, workplaces, communities, nbr_cells, nodes)
	for (count_type j = 0; j < NUM_PEOPLE; ++j){
	  update_lambdas(nodes[j], homes, workplaces, communities, nbr_cells, travel_fraction, time_step);
	}

	//Get data for this simulation step
	count_type n_infected = 0,
	  n_exposed = 0,
	  n_hospitalised = 0,
	  n_symptomatic = 0,
	  n_critical = 0,
	  n_fatalities = 0,
	  n_recovered = 0,
	  n_affected = 0,
	  n_infective = 0,
	  quarantined_individuals = 0,
	  quarantined_infectious = 0;

	count_type n_primary_contact = 0, 
	  n_mild_symptomatic_tested = 0, //CCC2
   	  n_moderate_symptomatic_tested = 0, //DCHC
      n_severe_symptomatic_tested = 0, //DCH
      n_icu = 0,
	  n_requested_tests = 0,
	  n_tested_positive = 0;

	double susceptible_lambda = 0,
	  susceptible_lambda_H = 0,
	  susceptible_lambda_W = 0,
	  susceptible_lambda_C = 0,
	  susceptible_lambda_T = 0,
	  susceptible_lambda_PROJECT = 0,
	  susceptible_lambda_NBR_CELL = 0,
	  susceptible_lambda_RANDOM_COMMUNITY = 0;
	double curtailed_interaction = 0, normal_interaction = 0;

#pragma omp parallel for default(none) shared(nodes, GLOBAL)			\
  reduction(+: n_infected, n_exposed,									\
			n_hospitalised, n_symptomatic,								\
			n_critical, n_fatalities,									\
			n_recovered, n_affected, n_infective,						\
			susceptible_lambda, susceptible_lambda_H,					\
			susceptible_lambda_W, susceptible_lambda_C,					\
			susceptible_lambda_T, susceptible_lambda_PROJECT,				\
			susceptible_lambda_NBR_CELL, susceptible_lambda_RANDOM_COMMUNITY,	 \
			quarantined_infectious, quarantined_individuals,			\
			curtailed_interaction, normal_interaction, 					\
	   		n_primary_contact, \
	  		n_mild_symptomatic_tested,  \
   	  		n_moderate_symptomatic_tested,  \
      		n_severe_symptomatic_tested, \
      		n_icu, n_requested_tests,n_tested_positive)
	for(count_type j = 0; j < NUM_PEOPLE; ++j){
	  auto infection_status = nodes[j].infection_status;
	  if(infection_status == Progression::susceptible){
		susceptible_lambda += nodes[j].lambda;
		susceptible_lambda_H += nodes[j].lambda_incoming.home;
		susceptible_lambda_W += nodes[j].lambda_incoming.work;
		susceptible_lambda_C += nodes[j].lambda_incoming.community;
		susceptible_lambda_T += nodes[j].lambda_incoming.travel;
		susceptible_lambda_PROJECT += nodes[j].lambda_incoming.project;
		susceptible_lambda_NBR_CELL += nodes[j].lambda_incoming.nbr_cell;
		susceptible_lambda_RANDOM_COMMUNITY += nodes[j].lambda_incoming.random_community;
	  }
	  if(infection_status == Progression::infective
		 || infection_status == Progression::symptomatic
		 || infection_status == Progression::hospitalised
		 || infection_status == Progression::critical){
		n_infected += 1;
	  }else if(infection_status != Progression::dead){
		  curtailed_interaction+=(nodes[j].kappa_H_incoming * GLOBAL.BETA_H
		  		+ nodes[j].kappa_C_incoming * GLOBAL.BETA_C
				+ ((nodes[j].workplace_type == WorkplaceType::office)?GLOBAL.BETA_W:0)*nodes[j].kappa_W_incoming
				+ ((nodes[j].workplace_type == WorkplaceType::school)?GLOBAL.BETA_S:0)*nodes[j].kappa_W_incoming
				+ ((nodes[j].workplace_type == WorkplaceType::office)?GLOBAL.BETA_PROJECT:0)*nodes[j].kappa_W_incoming
				+ ((nodes[j].workplace_type == WorkplaceType::school)?GLOBAL.BETA_CLASS:0)*nodes[j].kappa_W_incoming
				+ nodes[j].kappa_C_incoming*GLOBAL.BETA_NBR_CELLS
				+ nodes[j].kappa_C_incoming*GLOBAL.BETA_RANDOM_COMMUNITY
				+ ((nodes[j].has_to_travel)?GLOBAL.BETA_TRAVEL:0)*nodes[j].travels());
		  normal_interaction+=(GLOBAL.BETA_H
		  		+ GLOBAL.BETA_C
				+ ((nodes[j].workplace_type == WorkplaceType::office)?GLOBAL.BETA_W:0)
				+ ((nodes[j].workplace_type == WorkplaceType::school)?GLOBAL.BETA_S:0)
				+ ((nodes[j].workplace_type == WorkplaceType::office)?GLOBAL.BETA_PROJECT:0)
				+ ((nodes[j].workplace_type == WorkplaceType::school)?GLOBAL.BETA_CLASS:0)
				+ GLOBAL.BETA_NBR_CELLS
				+ GLOBAL.BETA_RANDOM_COMMUNITY
				+ ((nodes[j].has_to_travel)?GLOBAL.BETA_TRAVEL:0));
	  }
	  if(infection_status == Progression::exposed){
		n_exposed += 1;
	  }
	  if(infection_status == Progression::hospitalised){
		n_hospitalised += 1;
	  }
	  if(infection_status == Progression::symptomatic){
		n_symptomatic += 1;
	  }
	  if(infection_status == Progression::critical){
		n_critical += 1;
	  }
	  if(infection_status == Progression::dead){
		n_fatalities += 1;
	  }
	  if(infection_status == Progression::recovered){
		n_recovered += 1;
	  }
	  if(infection_status != Progression::susceptible){
		n_affected += 1;
	  }
	  if(nodes[j].infective){
		n_infective += 1;
	  }
	  if(nodes[j].quarantined){
		quarantined_individuals += 1;
	  }
	  if(nodes[j].quarantined && (infection_status == Progression::infective
								  || infection_status == Progression::symptomatic
								  || infection_status == Progression::hospitalised
								  || infection_status == Progression::critical)){
		quarantined_infectious += 1;
	  }

	  if(nodes[j].disease_label == DiseaseLabel::primary_contact){
		  n_primary_contact +=1;
	  }
	  if(nodes[j].disease_label == DiseaseLabel::mild_symptomatic_tested){
		  n_mild_symptomatic_tested +=1;
	  }
	  if(nodes[j].disease_label == DiseaseLabel::moderate_symptomatic_tested){
		  n_moderate_symptomatic_tested +=1;
	  }
      if(nodes[j].disease_label == DiseaseLabel::severe_symptomatic_tested){
		  n_severe_symptomatic_tested +=1;
	  }
	  if(nodes[j].disease_label == DiseaseLabel::icu){
		  n_icu +=1;
	  }
	  if(nodes[j].test_status.test_requested){
		  n_requested_tests +=1;
	  }
	  if(nodes[j].test_status.tested_positive){
		  n_tested_positive +=1;
	  }
	}

	//Apportion new expected infections (in next time step) to currently
	//infective nodes
	if(n_infective){
	  long double expected_infections_per_infective_node
		= (long double)(susceptible_lambda)/n_infective;
	  for(const auto& node: nodes){
		if(node.infective){
		  infections_by_new_infectives[node.time_became_infective]
			+= expected_infections_per_infective_node;
		}
	  }
	}

	plot_data.nums["num_infected"].push_back({time_step, {n_infected}});
	plot_data.nums["num_exposed"].push_back({time_step, {n_exposed}});
	plot_data.nums["num_hospitalised"].push_back({time_step, {n_hospitalised}});
	plot_data.nums["num_symptomatic"].push_back({time_step, {n_symptomatic}});
	plot_data.nums["num_critical"].push_back({time_step, {n_critical}});
	plot_data.nums["num_fatalities"].push_back({time_step, {n_fatalities}});
	plot_data.nums["num_recovered"].push_back({time_step, {n_recovered}});
	plot_data.nums["num_affected"].push_back({time_step, {n_affected}});
	plot_data.nums["num_cases"].push_back({time_step, {num_cases}});
	plot_data.nums["num_cumulative_hospitalizations"].push_back({time_step, {num_cumulative_hospitalizations}});
	plot_data.nums["num_cumulative_infective"].push_back({time_step, {num_cumulative_infective}});

	plot_data.susceptible_lambdas["susceptible_lambda"].push_back({time_step, {susceptible_lambda}});
	plot_data.susceptible_lambdas["susceptible_lambda_H"].push_back({time_step, {susceptible_lambda_H}});
	plot_data.susceptible_lambdas["susceptible_lambda_W"].push_back({time_step, {susceptible_lambda_W}});
	plot_data.susceptible_lambdas["susceptible_lambda_C"].push_back({time_step, {susceptible_lambda_C}});
	plot_data.susceptible_lambdas["susceptible_lambda_T"].push_back({time_step, {susceptible_lambda_T}});
	plot_data.susceptible_lambdas["susceptible_lambda_PROJECT"].push_back({time_step, {susceptible_lambda_PROJECT}});
	plot_data.susceptible_lambdas["susceptible_lambda_NBR_CELL"].push_back({time_step, {susceptible_lambda_NBR_CELL}});
	plot_data.susceptible_lambdas["susceptible_lambda_RANDOM_COMMUNITY"].push_back({time_step, {susceptible_lambda_RANDOM_COMMUNITY}});

	// disease label stats
	plot_data.disease_label_stats["disease_label_stats"].push_back({time_step, {n_primary_contact,
							n_mild_symptomatic_tested, n_moderate_symptomatic_tested, 
							n_severe_symptomatic_tested, n_icu, n_requested_tests, n_tested_positive}});

	//Convert to fraction
	auto total_lambda_fraction_data_sum = total_lambda_fraction_data.sum();
	total_lambda_fraction_data /= total_lambda_fraction_data_sum;


	plot_data.total_lambda_fractions["total_fraction_lambda_H"].push_back({time_step, {total_lambda_fraction_data.home}});
	plot_data.total_lambda_fractions["total_fraction_lambda_W"].push_back({time_step, {total_lambda_fraction_data.work}});
	plot_data.total_lambda_fractions["total_fraction_lambda_C"].push_back({time_step, {total_lambda_fraction_data.community}});
	plot_data.total_lambda_fractions["total_fraction_lambda_T"].push_back({time_step, {total_lambda_fraction_data.travel}});
	plot_data.total_lambda_fractions["total_fraction_lambda_PROJECT"].push_back({time_step, {total_lambda_fraction_data.project}});
	plot_data.total_lambda_fractions["total_fraction_lambda_NBR_CELL"].push_back({time_step, {total_lambda_fraction_data.nbr_cell}});
	plot_data.total_lambda_fractions["total_fraction_lambda_RANDOM_COMMUNITY"].push_back({time_step, {total_lambda_fraction_data.random_community}});

	plot_data.mean_lambda_fractions["mean_fraction_lambda_H"].push_back({time_step, {mean_lambda_fraction_data.home}});
	plot_data.mean_lambda_fractions["mean_fraction_lambda_W"].push_back({time_step, {mean_lambda_fraction_data.work}});
	plot_data.mean_lambda_fractions["mean_fraction_lambda_C"].push_back({time_step, {mean_lambda_fraction_data.community}});
	plot_data.mean_lambda_fractions["mean_fraction_lambda_T"].push_back({time_step, {mean_lambda_fraction_data.travel}});
	plot_data.mean_lambda_fractions["mean_fraction_lambda_PROJECT"].push_back({time_step, {mean_lambda_fraction_data.project}});
	plot_data.mean_lambda_fractions["mean_fraction_lambda_NBR_CELL"].push_back({time_step, {mean_lambda_fraction_data.nbr_cell}});
	plot_data.mean_lambda_fractions["mean_fraction_lambda_RANDOM_COMMUNITY"].push_back({time_step, {mean_lambda_fraction_data.random_community}});

	plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_H"].push_back({time_step,
																							   {cumulative_mean_lambda_fraction_data.home}});
	plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_W"].push_back({time_step,
																							   {cumulative_mean_lambda_fraction_data.work}});
	plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_C"].push_back({time_step,
																							   {cumulative_mean_lambda_fraction_data.community}});
	plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_T"].push_back({time_step,
																							   {cumulative_mean_lambda_fraction_data.travel}});
	plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_PROJECT"].push_back({time_step,
																							   {cumulative_mean_lambda_fraction_data.project}});
	plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_NBR_CELL"].push_back({time_step,
																							   {cumulative_mean_lambda_fraction_data.nbr_cell}});
	plot_data.cumulative_mean_lambda_fractions["cumulative_mean_fraction_lambda_RANDOM_COMMUNITY"].push_back({time_step,
																							   {cumulative_mean_lambda_fraction_data.random_community}});
	plot_data.quarantined_stats["quarantined_stats"].push_back({time_step, {
                 quarantined_individuals,
				 quarantined_infectious,
				 quarantined_num_cases
                  }});
	plot_data.curtailment_stats["curtailment_stats"].push_back({time_step, {
				 normal_interaction,
				 curtailed_interaction
                  }});
#ifdef DEBUG
	cerr<<std::endl<<"time_step: "<<time_step;
	auto end_time_timestep = std::chrono::high_resolution_clock::now();
  	cerr << "Time step: simulation time (ms): " << duration(start_time_timestep, end_time_timestep) << "\n";
#endif
  }

  //Create CSV data out of the date for infections per new infective node
  plot_data.infections_by_new_infectives = {
	{"infections_by_new_infectives", {}}
  };
  for(count_type time_step = 0; time_step < GLOBAL.NUM_TIMESTEPS; ++time_step){
	plot_data.infections_by_new_infectives["infections_by_new_infectives"].push_back({time_step,
																					  {infections_by_new_infectives[time_step]}});
  }

#ifdef TIMING
  end_time = std::chrono::high_resolution_clock::now();
  cerr << "simulator: simulation time (ms): " << duration(start_time, end_time) << "\n";
#endif
  

  return plot_data;
}

