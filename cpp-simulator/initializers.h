//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef INITIALIZERS_H_
#define INITIALIZERS_H_
#include "models.h"
#include <vector>


std::vector<house> init_homes();
std::vector<workplace> init_workplaces();
std::vector<community> init_community();
std::vector<agent> init_nodes();

matrix<double> compute_community_distances(const std::vector<community>& communities);

//Assign individuals to homes, workplace, community
void assign_individual_home_community(std::vector<agent>& nodes, std::vector<house>& homes, std::vector<workplace>& workplaces, std::vector<community>& communities);

// Compute scale factors for each home, workplace and community. Done once at the beginning.
void compute_scale_homes(std::vector<house>& homes);
void compute_scale_workplaces(std::vector<workplace>& workplaces);
void compute_scale_communities(const std::vector<agent>& nodes, std::vector<community>& communities);


#endif
