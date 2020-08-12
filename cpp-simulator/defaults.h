//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef DEFAULTS_H_
#define DEFAULTS_H_
#include <string>

struct defaults{
  std::string NUM_DAYS = "120";
  std::string INIT_FRAC_INFECTED = "0.0001";
  std::string INIT_FIXED_NUMBER_INFECTED = "100";
  std::string MEAN_INCUBATION_PERIOD = "4.50";
  std::string MEAN_ASYMPTOMATIC_PERIOD = "0.5";
  std::string MEAN_SYMPTOMATIC_PERIOD = "5";
  std::string SYMPTOMATIC_FRACTION = "0.67";
  std::string MEAN_HOSPITAL_REGULAR_PERIOD = "8";
  std::string MEAN_HOSPITAL_CRITICAL_PERIOD = "8";
  std::string COMPLIANCE_PROBABILITY = "0.9";
  std::string F_KERNEL_A = "10.751";
  std::string F_KERNEL_B = "5.384";
  std::string BETA_H = "1.227";
  std::string BETA_W = "0.919";
  std::string BETA_C = "0.233";
  std::string BETA_S = "1.820";
  std::string BETA_TRAVEL = "0.0";
  std::string HD_AREA_FACTOR = "2.0";
  std::string HD_AREA_EXPONENT = "0";
  std::string INTERVENTION = "0";
  std::string CYCLIC_POLICY_TYPE = "0";
  std::string output_dir = "outputs/test_output_timing";
  std::string input_base = "../simulator/input_files";
  std::string attendance_filename = "attendance.json";
  std::string SEED_HD_AREA_POPULATION = "false";
  std::string SEED_ONLY_NON_COMMUTER = "false";
  std::string SEED_FIXED_NUMBER = "false";
  std::string IGNORE_ATTENDANCE_FILE = "true";
  std::string CALIBRATION_DELAY = "0";
  std::string DAYS_BEFORE_LOCKDOWN = "0";
  std::string FIRST_PERIOD = "21";
  std::string SECOND_PERIOD = "21";
  std::string THIRD_PERIOD = "42";
  std::string OE_SECOND_PERIOD = "30";
  std::string LOCKED_COMMUNITY_LEAKAGE = "1.0";
  std::string COMMUNITY_LOCK_THRESHOLD = "0.001";
  std::string LOCKED_NEIGHBORHOOD_LEAKAGE = "1.0";
  std::string NEIGHBORHOOD_LOCK_THRESHOLD = "0.001";
  std::string MASK_ACTIVE = "false";
  std::string MASK_FACTOR = "0.8";
  std::string MASK_START_DELAY = "40";
  std::string USE_AGE_DEPENDENT_MIXING = "false";
  std::string SIGNIFICANT_EIGEN_VALUES = "3.0";
  std::string NUM_AGE_GROUPS = "16";
  std::string CITY_SW_LAT = "12.8340125";
  std::string CITY_SW_LON = "77.46010253";
  std::string CITY_NE_LAT = "13.14366615";
  std::string CITY_NE_LON = "77.78405146";
  std::string NBR_CELL_SIZE = "0.178"; // in km
  std::string ENABLE_CONTAINMENT = "false";
  std::string ENABLE_NBR_CELLS = "false";
  std::string ENABLE_NEIGHBORHOOD_SOFT_CONTAINMENT = "false";
  std::string WARD_CONTAINMENT_THRESHOLD  = "0";
  std::string intervention_params_filename = "intervention_params.json";
  std::string BETA_PROJECT = "0";
  std::string BETA_CLASS = "0";
  std::string BETA_RANDOM_COMMUNITY = "0";
  std::string BETA_NBR_CELLS = "0";
  std::string ENABLE_TESTING = "false";
  std::string TESTING_PROTOCOL = "0";
  std::string testing_protocol_filename = "testing_protocol.json";
} DEFAULTS;

#endif
