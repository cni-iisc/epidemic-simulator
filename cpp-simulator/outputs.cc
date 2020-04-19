//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#include "models.h"
#include "outputs.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

using std::string;
using std::vector;
using std::endl;
using std::cerr;

void check_stream(const std::ofstream& fout, const std::string& path){
  if(!fout){
	cerr << "simulator: could not open file "
		 << path << "\n"
		 << "simulator: please make sure the directory exists\n";
	exit(1);
  }
}


string intervention_rep(Intervention i){
  switch(i){
  case Intervention::no_intervention:
	return "no_intervention";
	break;
  case Intervention::case_isolation:
	return "case_isolation";
	break;
  case Intervention::home_quarantine:
	return "home_quarantine";
	break;
  case Intervention::lockdown:
	return "lockdown";
	break;
  case Intervention::case_isolation_and_home_quarantine:
	return "case_isolation_and_home_quarantine";
	break;
  case Intervention::case_isolation_and_home_quarantine_sd_65_plus:
	return "case_isolation_and_home_quarantine_sd_65_plus";
	break;
  case Intervention::lockdown_fper_ci_hq_sd_65_plus_sper_ci:
	return "lockdown_fper_ci_hq_sd_65_plus_sper_ci";
	break;
  case Intervention::lockdown_fper:
	return "lockdown_fper";
  case Intervention::ld_fper_ci_hq_sd65_sc_sper_sc_tper:
	return "ld_fper_ci_hq_sd65_sc_sper_sc_tper";
	break;
  case Intervention::ld_fper_ci_hq_sd65_sc_sper:
	return "ld_fper_ci_hq_sd65_sc_sper";
	break;
  case Intervention::ld_fper_ci_hq_sd65_sc_oe_sper:
	return "ld_fper_ci_hq_sd65_sc_oe_sper";
	break;
  default:
	assert(false);
	break;
  }
}

const std::string CSV_TERM = "\n";
const char CSV_SEP = ',';
template <class T>
void output_timed_csv(const std::vector<std::string>& field_row, const std::string& output_file, const timed_csv_data<T>& mat){
  std::ofstream fout(output_file, std::ios::out);
  check_stream(fout, output_file);

  fout << "Time" << CSV_SEP;
  auto end = field_row.end();
  auto penultimate = end - 1;
  for(auto it = field_row.begin(); it != end; ++it){
	fout << *it;
	if(it != penultimate){
	  fout<< CSV_SEP;
	}
  }
  fout << CSV_TERM;
  for(const auto& row: mat){
	auto end = std::get<1>(row).end();
	auto penultimate = end - 1;
	fout << double(std::get<0>(row))/GLOBAL.SIM_STEPS_PER_DAY << CSV_SEP;
	for(auto it = std::get<1>(row).begin(); it < end; ++it){
	  fout << *it;
	  if(it != penultimate){
		fout<< CSV_SEP;
	  }
	}
	fout << CSV_TERM;
  }
  fout.close();
}

void output_global_params(const string& output_dir){
  std::string global_params_path = output_dir + "/global_params.txt";
  std::ofstream fout(output_dir + "/global_params.txt", std::ios::out);
  check_stream(fout, global_params_path);

  fout << "COMPLIANCE_PROBABILITY: " << GLOBAL.COMPLIANCE_PROBABILITY << ";" << endl; 
  
  fout << "num_homes: " << GLOBAL.num_homes << ";" << endl; 
  fout << "num_workplaces: " << GLOBAL.num_workplaces << ";" << endl; 
  fout << "num_schools: " << GLOBAL.num_schools << ";" << endl; 
  fout << "num_communities: " << GLOBAL.num_communities << ";" << endl; 
  
  fout << "num_people: " << GLOBAL.num_people << ";" << endl; 
  
  fout << "NUM_DAYS: " << GLOBAL.NUM_DAYS << ";" << endl;  //Number of days. Simulation duration
  fout << "SIM_STEPS_PER_DAY: " << GLOBAL.SIM_STEPS_PER_DAY << ";" << endl;  //Number of simulation steps per day.
  fout << "NUM_TIMESTEPS: " << GLOBAL.NUM_TIMESTEPS << ";" << endl;  //

  fout << "SEED_FIXED_NUMBER: " << GLOBAL.SEED_FIXED_NUMBER << "; " << endl;
  fout << "INIT_FRAC_INFECTED: " << GLOBAL.INIT_FRAC_INFECTED << ";" << endl;  // Initial number of people infected
  if(GLOBAL.SEED_FIXED_NUMBER){
	fout << "#Since SEED_FIXED_NUMBER is set, INIT_FRAC_INFECTED is overridden by INIT_FIXED_NUMBER_INFECTED\n";
	fout << "INIT_FIXED_NUMBER_INFECTED: " << GLOBAL.INIT_FIXED_NUMBER_INFECTED  << ";" << endl;
  }
  fout << "#Actual number of initial infections: " << GLOBAL.INIT_ACTUALLY_INFECTED << ";" << endl;

  
  fout << "INCUBATION_PERIOD: " << GLOBAL.INCUBATION_PERIOD << ";" << endl; 
  fout << "MEAN_ASYMPTOMATIC_PERIOD: " << GLOBAL.MEAN_ASYMPTOMATIC_PERIOD << ";" << endl; 
  fout << "MEAN_SYMPTOMATIC_PERIOD: " << GLOBAL.MEAN_SYMPTOMATIC_PERIOD << ";" << endl; 
  fout << "MEAN_HOSPITAL_REGULAR_PERIOD: " << GLOBAL.MEAN_HOSPITAL_REGULAR_PERIOD << ";" << endl; 
  fout << "MEAN_HOSPITAL_CRITICAL_PERIOD: " << GLOBAL.MEAN_HOSPITAL_CRITICAL_PERIOD << ";" << endl; 
  
  fout << "INCUBATION_PERIOD_SHAPE: " << GLOBAL.INCUBATION_PERIOD_SHAPE << ";" << endl; 
  fout << "INCUBATION_PERIOD_SCALE: " << GLOBAL.INCUBATION_PERIOD_SCALE << ";" << endl;
  
  fout << "INFECTIOUSNESS_SHAPE: " << GLOBAL.INFECTIOUSNESS_SHAPE << ";" << endl; 
  fout << "INFECTIOUSNESS_SCALE: " << GLOBAL.INFECTIOUSNESS_SCALE << ";" << endl; 
  
  fout << "SEVERITY_RATE: " << GLOBAL.SEVERITY_RATE << ";" << endl;  //value used in sim.js

  fout << "ASYMPTOMATIC_PERIOD: " << GLOBAL.ASYMPTOMATIC_PERIOD << ";" << endl; 
  // half a day
  fout << "SYMPTOMATIC_PERIOD: " << GLOBAL.SYMPTOMATIC_PERIOD << ";" << endl; 
  // 5 days
  fout << "HOSPITAL_REGULAR_PERIOD: " << GLOBAL.HOSPITAL_REGULAR_PERIOD << ";" << endl; 
  fout << "HOSPITAL_CRITICAL_PERIOD: " << GLOBAL.HOSPITAL_CRITICAL_PERIOD << ";" << endl; 
  fout << "SYMPTOMATIC_FRACTION: " << GLOBAL.SYMPTOMATIC_FRACTION << ";" << endl; 

  fout << "INTERVENTION: " << static_cast<count_type>(GLOBAL.INTERVENTION) << ";" << endl; 

  // Beta values
  fout << "BETA_H: " << GLOBAL.BETA_H << ";" << endl;  //Thailand data
  fout << "BETA_W: " << GLOBAL.BETA_W << ";" << endl;  //Thailand data
  fout << "BETA_S: " << GLOBAL.BETA_S << ";" << endl;  //Thailand data
  fout << "BETA_C: " << GLOBAL.BETA_C << ";" << endl;  // Thailand data. Product = 0.47

  fout << "ALPHA: " << GLOBAL.ALPHA << ";" << endl; 

  //Transport
  fout << "BETA_TRAVEL: " << GLOBAL.BETA_TRAVEL << ";" << endl;
  fout << "P_TRAIN: " << GLOBAL.P_TRAIN << ";" << endl;

  //Multiplicative fatcor for infection rates in high density areas
  fout << "HD_AREA_FACTOR: " << GLOBAL.HD_AREA_FACTOR << ";" << endl;
  fout << "HD_AREA_EXPONENT: " << GLOBAL.HD_AREA_EXPONENT << ";" << endl;

  //Details on interventions
  fout << "CALIBRATION_DELAY: " << GLOBAL.CALIBRATION_DELAY << ";" << endl;;
  fout << "DAYS_BEFORE_LOCKDOWN: " << GLOBAL.DAYS_BEFORE_LOCKDOWN << ";" << endl;;
  fout << "FIRST_PERIOD: " << GLOBAL.FIRST_PERIOD << ";" << endl;;
  fout << "SECOND_PERIOD: " << GLOBAL.SECOND_PERIOD << ";" << endl;;
  fout << "THIRD_PERIOD: " << GLOBAL.THIRD_PERIOD << ";" << endl;;
  fout << "OE_SECOND_PERIOD: " << GLOBAL.OE_SECOND_PERIOD << ";" << endl;;
  
  fout << "USE_SAME_INFECTION_PROB_FOR_ALL_WARDS: " << GLOBAL.USE_SAME_INFECTION_PROB_FOR_ALL_WARDS << ";" << endl;
  fout << "SEED_HD_AREA_POPULATION: " << GLOBAL.SEED_HD_AREA_POPULATION << ";" << endl;
  fout << "SEED_ONLY_NON_COMMUTER: " << GLOBAL.SEED_ONLY_NON_COMMUTER << ";" << endl;
  
  fout.close();
}


gnuplot::gnuplot(const std::string& output_directory){
  std::string gnuplot_script_path = output_directory + "/gnuplot_script.gnuplot";
  fout.open(gnuplot_script_path);
  check_stream(fout, gnuplot_script_path);

  std::string plots_path = output_directory + "/plots.html";
  html_out.open(plots_path);
  check_stream(html_out, plots_path);
  
  fout << "set datafile separator ','" << std::endl;
  fout << "set key autotitle columnhead" << std::endl;
  fout << "set term png" << std::endl;
  fout << "set termoption noenhanced" << std::endl;
  auto intervention = intervention_rep(GLOBAL.INTERVENTION);
  html_out << "<html>\n<head><title>Plots: Intervention = "
		   << intervention
		   << "</title></head>\n<body>\n"
	       << "<h1>Intervention = " << intervention << "</h1>\n";
}

void gnuplot::plot_data(const string& name){
  auto image_name = name + ".png";
  fout << "set output \"" << image_name << "\"" << std::endl;
  fout << "set title \"" <<  name << "\"" << std::endl;
  fout << "plot \"" << name << ".csv\" using 1:2 with lines" << std::endl;
  html_out << "<p><img src=\"" << image_name  << "\">\n";
}

gnuplot::~gnuplot(){
  fout.close();
  html_out << "\n</body>\n</html>\n";
  html_out.close();
}


void output_csv_files(const std::string& output_directory,
					  gnuplot& gnuplot,
					  const plot_data_struct& plot_data){
  for(const auto& elem: plot_data.nums){
	std::string csvfile_name = elem.first + ".csv";
	std::string csvfile_path = output_directory + "/" + csvfile_name;
	if(elem.first == "csvContent"){
	  //This file contains everything!
	  output_timed_csv({"community",
						"infected",
						"affected",
						"hospitalised",
						"critical",
						"dead",
						"hd_area_affected"},
		csvfile_path, elem.second);
	} else {
	  output_timed_csv({elem.first},
					   csvfile_path,
					   elem.second);
	  gnuplot.plot_data(elem.first);
	}
  }

  //Now output lambdas
  for(const auto& elem: plot_data.susceptible_lambdas){
	std::string csvfile_name = elem.first + ".csv";
	std::string csvfile_path = output_directory + "/" + csvfile_name;
	output_timed_csv({elem.first},
					 csvfile_path,
					 elem.second);
	gnuplot.plot_data(elem.first);
  }

  //Now output fractional lambda contributions: total version
  for(const auto& elem: plot_data.total_lambda_fractions){
	std::string csvfile_name = elem.first + ".csv";
	std::string csvfile_path = output_directory + "/" + csvfile_name;
	output_timed_csv({elem.first},
					 csvfile_path,
					 elem.second);
	gnuplot.plot_data(elem.first);
  }
  
  //Now output fractional lambda contributions: mean version
  for(const auto& elem: plot_data.mean_lambda_fractions){
	std::string csvfile_name = elem.first + ".csv";
	std::string csvfile_path = output_directory + "/" + csvfile_name;
	output_timed_csv({elem.first},
					 csvfile_path,
					 elem.second);
	gnuplot.plot_data(elem.first);
  }
  
  //Now output fractional lambda contributions: cumulative mean version
  for(const auto& elem: plot_data.cumulative_mean_lambda_fractions){
	std::string csvfile_name = elem.first + ".csv";
	std::string csvfile_path = output_directory + "/" + csvfile_name;
	output_timed_csv({elem.first},
					 csvfile_path,
					 elem.second);
	gnuplot.plot_data(elem.first);
  }
}
