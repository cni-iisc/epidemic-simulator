//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef INTERVENTION_PRIMITIVES_H_
#define INTERVENTION_PRIMITIVES_H_
#include "models.h"
#include <vector>

constexpr int UPPER_AGE = 65;

void set_kappa_base_node(agent& node, double community_factor, int cur_time);

void set_kappa_lockdown_node(agent& node, int cur_time, const intervention_params intv_params);

void modify_kappa_SDE_node(agent& node);

void modify_kappa_SC_node(agent& node, double SC_factor);

void modify_kappa_OE_node(agent& node);

void modify_kappa_ward_containment(agent& node);

void reset_home_quarantines(std::vector<house>& homes);

void modify_kappa_case_isolate_node(agent& node);

bool should_be_isolated_node(const agent& node, const int cur_time, const int quarantine_days);

void mark_and_isolate_quarantined_homes(std::vector<agent>& nodes, std::vector<house>& homes, int cur_time);

void mark_homes_for_quarantine(const std::vector<agent>& nodes, std::vector<house>& homes, int cur_time);

void mark_neighbourhood_homes_for_quarantine(const std::vector<agent>& nodes, std::vector<house>& homes,
											 const matrix<nbr_cell>& nbr_cells, int cur_time);

#endif

