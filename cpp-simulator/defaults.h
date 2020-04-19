//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef DEFAULTS_H_
#define DEFAULTS_H_
#include <string>

struct defaults{
  std::string NUM_DAYS = "120";
  std::string INIT_FRAC_INFECTED = "0.0001";
  std::string INIT_FIXED_NUMBER_INFECTED = "100";
  std::string INCUBATION_PERIOD = "2.25";
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
  std::string output_dir = "outputs/test_output_timing";
  std::string input_base = "../simulator/input_files";
  std::string SEED_HD_AREA_POPULATION = "false";
  std::string SEED_ONLY_NON_COMMUTER = "false";
  std::string SEED_FIXED_NUMBER = "false";
  std::string CALIBRATION_DELAY = "0";
  std::string DAYS_BEFORE_LOCKDOWN = "0";
  std::string FIRST_PERIOD = "21";
  std::string SECOND_PERIOD = "21";
  std::string THIRD_PERIOD = "42";
  std::string OE_SECOND_PERIOD = "30";
} DEFAULTS;

#endif
