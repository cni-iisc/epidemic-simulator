//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef UPDATES_H_
#define UPDATES_H_

#include "models.h"
#include <vector>

double update_individual_lambda_h(const agent& node);

double update_individual_lambda_w(const agent& node);

double update_individual_lambda_c(const agent& node);

struct node_update_status{
  bool new_infection = false;
  bool new_symptomatic = false;
  bool new_hospitalization = false;
};

//Returns whether the node was infected or turned symptomatic in this time step
node_update_status update_infection(agent& node, int cur_time);

void update_all_kappa(std::vector<agent>& nodes, std::vector<house>& homes, std::vector<workplace>& workplaces, std::vector<community>& communities, int cur_time);

double updated_lambda_w_age_independent(const std::vector<agent>& nodes, const workplace& workplace);

double updated_lambda_h_age_independent(const std::vector<agent>& nodes, const house& home);

double updated_travel_fraction(const std::vector<agent>& nodes);

void update_lambdas(agent&node, const std::vector<house>& homes, const std::vector<workplace>& workplaces, const std::vector<community>& communities, double travel_fraction, int cur_time);

double updated_lambda_c_local(const std::vector<agent>& nodes, const community& community);

void update_lambda_c_global(std::vector<community>& communities, const matrix<double>& community_distance_matrix);

struct casualty_stats{
  count_type infected = 0;
  count_type affected = 0;
  count_type hospitalised = 0;
  count_type critical = 0;
  count_type dead = 0;
  count_type exposed = 0;
  count_type recovered = 0;
  count_type hd_area_affected = 0;
};

casualty_stats get_infected_community(const std::vector<agent>& nodes, const community& community);

#endif
