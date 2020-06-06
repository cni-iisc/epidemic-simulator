//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef INITIALIZERS_H_
#define INITIALIZERS_H_
#include "models.h"
#include <vector>

//Initialize the office attendance
void initialize_office_attendance();

std::vector<house> init_homes();
std::vector<workplace> init_workplaces();
std::vector<community> init_community();
std::vector<agent> init_nodes();
matrix<nbr_cell> init_nbr_cells();
std::vector<intervention_params> init_intervention_params();

matrix<double> compute_community_distances(const std::vector<community>& communities);
matrix<double> compute_community_distances_fkernel(const matrix<double>& community_distances);

//Assign individuals to homes, workplace, community
void assign_individual_home_community(std::vector<agent>& nodes, std::vector<house>& homes, std::vector<workplace>& workplaces, std::vector<community>& communities);
void assign_homes_nbr_cell(const std::vector<house>& homes, matrix<nbr_cell>& nbr_cells);
// Compute scale factors for each home, workplace and community. Done once at the beginning.
void compute_scale_homes(std::vector<house>& homes);
void compute_scale_workplaces(std::vector<workplace>& workplaces);
void compute_scale_communities(const std::vector<agent>& nodes, std::vector<community>& communities);

// Age stratification JSON read function.
svd init_home_age_interaction_matrix();
svd init_school_age_interaction_matrix();
svd init_workplace_age_interaction_matrix();
svd init_community_age_interaction_matrix();


#endif
