#ifndef COHORTS_H_
#define COHORTS_H_
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <cmath>
#include <set>
#include <unordered_map>

#include "models.h"
#include "train_loader.h"

TrainLoader init_TrainLoader();

void storeAgentsInfo(const std::vector<agent>& nodes);
bool loadAgentsInfo(std::vector<agent>& nodes);

std::unordered_map<count_type, std::vector<cohort_space>> make_oneoff_cohorts(
    std::vector<agent>& nodes, const TrainLoader& trains);

void merge_coaches(std::unordered_map<count_type, std::vector<train_coach>>& to,
    const std::unordered_map<count_type, std::vector<train_coach>>& from);

void merge_cohorts(std::unordered_map<count_type, std::vector<cohort_space>>& to,
   const std::unordered_map<count_type, std::vector<cohort_space>>& from);

std::unordered_map<count_type, std::vector<cohort_space>> make_cohorts(
  std::vector<agent>& nodes,
  count_type target_size,
  const TrainLoader& trains);

std::unordered_map<count_type, std::vector<cohort_space>> make_random_cohorts(
  std::vector<agent>& nodes,
  count_type target_size);

std::unordered_map<count_type, std::vector<train_coach>> make_train_coaches(
  std::unordered_map<count_type, std::vector<cohort_space>>& cohorts,
  const TrainLoader& trains,
  bool isAM, bool isOneOff);

void compute_scale_intra_cohorts(
  std::unordered_map<count_type, std::vector<cohort_space>>& cohorts,
  const std::vector<agent>& nodes);

void assign_inter_cohort(
  std::unordered_map<count_type, std::vector<train_coach>>& coachs,
  const std::vector<agent>& nodes);

void update_kappas_cohorts(
  std::unordered_map<count_type, std::vector<cohort_space>>& cohorts,
  std::vector<agent>& nodes,
  int cur_time);

int get_num_cohorts(const std::unordered_map<count_type, std::vector<cohort_space>>& cohorts);

std::vector<std::tuple<int, bool, int, double>> get_coach_occupancy_per_line(
  const std::unordered_map<count_type, std::vector<train_coach>>& coachs);

#endif
