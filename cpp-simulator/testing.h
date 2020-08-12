//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef TESTING_H_
#define TESTING_H_
#include "models.h"
#include <vector>


struct node_update_status_testing{
  bool new_hospitalization = false;
};

void set_test_request(std::vector<agent>& nodes, const std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<std::vector<nbr_cell>>& nbr_cells, const std::vector<community>& communities, const testing_probability probabilities, const count_type current_time);
void update_infection_testing(std::vector<agent>& nodes, std::vector<house>& homes, count_type current_time);
void set_test_request_fileread(std::vector<agent>& nodes, const std::vector<house>& homes,
						 const std::vector<workplace>& workplaces, const matrix<nbr_cell>& nbr_cells,
						 const std::vector<community>& communities,						 
						 const std::vector<testing_probability>& testing_probability_vector, const int cur_time);
void test_contact_trace_household(count_type node_index, std::vector<agent>& nodes, const std::vector<house>& homes, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time );
void test_contact_trace_project(count_type node_index, std::vector<agent>& nodes, const std::vector<workplace>& workplaces, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time );
void test_contact_trace_random_community(count_type node_index, std::vector<agent>& nodes, const std::vector<house>& homes, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time );
void test_contact_trace_neighbourhood_cell(count_type node_index, std::vector<agent>& nodes, const std::vector<house>& homes, matrix<nbr_cell> nbr_cells, double probability_contact_trace, double probability_test_symptomatic, double probability_test_asymptomatic, const count_type current_time );

#endif
