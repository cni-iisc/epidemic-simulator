set datafile separator ','
set key autotitle columnhead
set term png
set termoption noenhanced
set output "num_affected.png"
set title "num_affected"
plot "num_affected.csv" using 1:2 with lines
set output "num_cases.png"
set title "num_cases"
plot "num_cases.csv" using 1:2 with lines
set output "num_critical.png"
set title "num_critical"
plot "num_critical.csv" using 1:2 with lines
set output "num_cumulative_hospitalizations.png"
set title "num_cumulative_hospitalizations"
plot "num_cumulative_hospitalizations.csv" using 1:2 with lines
set output "num_cumulative_infective.png"
set title "num_cumulative_infective"
plot "num_cumulative_infective.csv" using 1:2 with lines
set output "num_exposed.png"
set title "num_exposed"
plot "num_exposed.csv" using 1:2 with lines
set output "num_fatalities.png"
set title "num_fatalities"
plot "num_fatalities.csv" using 1:2 with lines
set output "num_hospitalised.png"
set title "num_hospitalised"
plot "num_hospitalised.csv" using 1:2 with lines
set output "num_infected.png"
set title "num_infected"
plot "num_infected.csv" using 1:2 with lines
set output "num_recovered.png"
set title "num_recovered"
plot "num_recovered.csv" using 1:2 with lines
set output "num_symptomatic.png"
set title "num_symptomatic"
plot "num_symptomatic.csv" using 1:2 with lines
set output "susceptible_lambda.png"
set title "susceptible_lambda"
plot "susceptible_lambda.csv" using 1:2 with lines
set output "susceptible_lambda_C.png"
set title "susceptible_lambda_C"
plot "susceptible_lambda_C.csv" using 1:2 with lines
set output "susceptible_lambda_H.png"
set title "susceptible_lambda_H"
plot "susceptible_lambda_H.csv" using 1:2 with lines
set output "susceptible_lambda_NBR_CELL.png"
set title "susceptible_lambda_NBR_CELL"
plot "susceptible_lambda_NBR_CELL.csv" using 1:2 with lines
set output "susceptible_lambda_PROJECT.png"
set title "susceptible_lambda_PROJECT"
plot "susceptible_lambda_PROJECT.csv" using 1:2 with lines
set output "susceptible_lambda_RANDOM_COMMUNITY.png"
set title "susceptible_lambda_RANDOM_COMMUNITY"
plot "susceptible_lambda_RANDOM_COMMUNITY.csv" using 1:2 with lines
set output "susceptible_lambda_T.png"
set title "susceptible_lambda_T"
plot "susceptible_lambda_T.csv" using 1:2 with lines
set output "susceptible_lambda_W.png"
set title "susceptible_lambda_W"
plot "susceptible_lambda_W.csv" using 1:2 with lines
set output "total_fraction_lambda_C.png"
set title "total_fraction_lambda_C"
plot "total_fraction_lambda_C.csv" using 1:2 with lines
set output "total_fraction_lambda_H.png"
set title "total_fraction_lambda_H"
plot "total_fraction_lambda_H.csv" using 1:2 with lines
set output "total_fraction_lambda_NBR_CELL.png"
set title "total_fraction_lambda_NBR_CELL"
plot "total_fraction_lambda_NBR_CELL.csv" using 1:2 with lines
set output "total_fraction_lambda_PROJECT.png"
set title "total_fraction_lambda_PROJECT"
plot "total_fraction_lambda_PROJECT.csv" using 1:2 with lines
set output "total_fraction_lambda_RANDOM_COMMUNITY.png"
set title "total_fraction_lambda_RANDOM_COMMUNITY"
plot "total_fraction_lambda_RANDOM_COMMUNITY.csv" using 1:2 with lines
set output "total_fraction_lambda_T.png"
set title "total_fraction_lambda_T"
plot "total_fraction_lambda_T.csv" using 1:2 with lines
set output "total_fraction_lambda_W.png"
set title "total_fraction_lambda_W"
plot "total_fraction_lambda_W.csv" using 1:2 with lines
set output "mean_fraction_lambda_C.png"
set title "mean_fraction_lambda_C"
plot "mean_fraction_lambda_C.csv" using 1:2 with lines
set output "mean_fraction_lambda_H.png"
set title "mean_fraction_lambda_H"
plot "mean_fraction_lambda_H.csv" using 1:2 with lines
set output "mean_fraction_lambda_NBR_CELL.png"
set title "mean_fraction_lambda_NBR_CELL"
plot "mean_fraction_lambda_NBR_CELL.csv" using 1:2 with lines
set output "mean_fraction_lambda_PROJECT.png"
set title "mean_fraction_lambda_PROJECT"
plot "mean_fraction_lambda_PROJECT.csv" using 1:2 with lines
set output "mean_fraction_lambda_RANDOM_COMMUNITY.png"
set title "mean_fraction_lambda_RANDOM_COMMUNITY"
plot "mean_fraction_lambda_RANDOM_COMMUNITY.csv" using 1:2 with lines
set output "mean_fraction_lambda_T.png"
set title "mean_fraction_lambda_T"
plot "mean_fraction_lambda_T.csv" using 1:2 with lines
set output "mean_fraction_lambda_W.png"
set title "mean_fraction_lambda_W"
plot "mean_fraction_lambda_W.csv" using 1:2 with lines
set output "cumulative_mean_fraction_lambda_C.png"
set title "cumulative_mean_fraction_lambda_C"
plot "cumulative_mean_fraction_lambda_C.csv" using 1:2 with lines
set output "cumulative_mean_fraction_lambda_H.png"
set title "cumulative_mean_fraction_lambda_H"
plot "cumulative_mean_fraction_lambda_H.csv" using 1:2 with lines
set output "cumulative_mean_fraction_lambda_NBR_CELL.png"
set title "cumulative_mean_fraction_lambda_NBR_CELL"
plot "cumulative_mean_fraction_lambda_NBR_CELL.csv" using 1:2 with lines
set output "cumulative_mean_fraction_lambda_PROJECT.png"
set title "cumulative_mean_fraction_lambda_PROJECT"
plot "cumulative_mean_fraction_lambda_PROJECT.csv" using 1:2 with lines
set output "cumulative_mean_fraction_lambda_RANDOM_COMMUNITY.png"
set title "cumulative_mean_fraction_lambda_RANDOM_COMMUNITY"
plot "cumulative_mean_fraction_lambda_RANDOM_COMMUNITY.csv" using 1:2 with lines
set output "cumulative_mean_fraction_lambda_T.png"
set title "cumulative_mean_fraction_lambda_T"
plot "cumulative_mean_fraction_lambda_T.csv" using 1:2 with lines
set output "cumulative_mean_fraction_lambda_W.png"
set title "cumulative_mean_fraction_lambda_W"
plot "cumulative_mean_fraction_lambda_W.csv" using 1:2 with lines
set output "infections_by_new_infectives.png"
set title "infections_by_new_infectives"
plot "infections_by_new_infectives.csv" using 1:2 with lines
