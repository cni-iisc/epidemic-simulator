//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef OUTPUTS_H_
#define OUTPUTS_H_
#include "models.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <tuple>

template <class T>
using timed_csv_row = std::tuple<count_type, std::vector<T>>;

template <class T>
using timed_csv_data = std::vector<timed_csv_row<T>>;


struct plot_data_struct{
  std::map<std::string, timed_csv_data<count_type>> nums;
  std::map<std::string, timed_csv_data<double>> susceptible_lambdas;
  std::map<std::string, timed_csv_data<double>> total_lambda_fractions;
  std::map<std::string, timed_csv_data<double>> mean_lambda_fractions;
  std::map<std::string, timed_csv_data<double>> cumulative_mean_lambda_fractions;
};


template <class T>
void output_timed_csv(const std::vector<std::string>& field_row, const std::string& output_file, const timed_csv_data<T>& mat);

void output_global_params(const std::string& filename);

struct gnuplot{
public:
  gnuplot(const std::string& output_directory);
  void plot_data(const std::string& name);
  ~gnuplot();
  
private:
  std::ofstream fout;
  std::ofstream html_out;
};

void output_csv_files(const std::string& output_directory, gnuplot& gnuplot, const plot_data_struct& plot_data);

void check_stream(const std::ofstream& fout, const std::string& path);

#endif
