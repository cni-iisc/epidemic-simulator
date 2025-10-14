#Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
#SPDX-License-Identifier: Apache-2.0
import numpy as np
#INPUT SECTION
#Edit this to change parameters
options = {
    'NUM_DAYS': '120',
    'INIT_FRAC_INFECTED': '0.0001',
    'INCUBATION_PERIOD': '2.25',
    'MEAN_ASYMPTOMATIC_PERIOD': '0.5',
    'MEAN_SYMPTOMATIC_PERIOD': '5',
    'SYMPTOMATIC_FRACTION': '0.67',
    'MEAN_HOSPITAL_REGULAR_PERIOD': '8',
    'MEAN_HOSPITAL_CRITICAL_PERIOD': '8',
    'COMPLIANCE_PROBABILITY': '0.9',
    'BETA_H': 1.227,
    'BETA_W': 0.919,
    'BETA_C': 0.233,
    'BETA_S': 1.820,
    'BETA_TRAVEL': 0,
    'HD_AREA_FACTOR': '2.0',
    'HD_AREA_EXPONENT': '0',
    'INTERVENTION': '0',
    'output_directory': "outputs/test_output_timing",
    'input_directory': "../simulator/input_files",
    'CALIBRATION_DELAY': '0',
    'DAYS_BEFORE_LOCKDOWN': '0',
    # Set this to "true" to seed hd area population as well.
    # SEED_HD_AREA_POPULATION='true',
    'SEED_HD_AREA_POPULATION': 'false',
    # Set this to "true" to seed only those who do not
    # take public transit
    # SEED_ONLY_NON_COMMUTER: "true"
    'SEED_ONLY_NON_COMMUTER': 'false',
    # Set this to "true" to seed only a fixed number of people. In
    # this case, the value of INIT_FRAC_INFECTED will be ignored in
    # favour of the value of INIT_FIXED_NUMBER_INFECTED
    # 'SEED_FIXED_NUMBER': 'true',
    'SEED_FIXED_NUMBER': 'false',
    'INIT_FIXED_NUMBER_INFECTED': '100',
}

grid_points = np.linspace(start = 0.8, stop = 1.2, num = 5)

BETA_H_VALUES = grid_points*options["BETA_H"]
BETA_W_VALUES = grid_points*options["BETA_W"]
BETA_S_VALUES = grid_points*options["BETA_S"]
BETA_C_VALUES = grid_points*options["BETA_C"]

#Do not edit below this line for editing inputs
import argparse
import csv
import logging
import matplotlib.pyplot as plt
import pandas as pd
from pathlib import Path
import subprocess
import sys

from sklearn.linear_model import LinearRegression
import sklearn.metrics as metrics

parser = argparse.ArgumentParser(description = "fit betas to slope of semi-log fatality plot")
parser.add_argument("input_directory", type=str,
                    help="the directory from which to read the input files")
parser.add_argument("output_directory", type=str,
                    help="the directory where the the output files will be written")
parser.add_argument("--lower-end-of-fatality-curve", '-l', type=int, default = 10,
                    dest = "min_fatalities",
                    help="total fatalities at time at which regression starts (default: 10)")
parser.add_argument("--upper-end-of-fatality-curve", '-u', type=int, default = 200,
                    dest = "max_fatalities",
                    help="total fatalities at time at which regression stops (default: 200)")


args = parser.parse_args()
options["input_directory"] = args.input_directory
options["output_directory"] = args.output_directory
min_fatalities = args.min_fatalities
max_fatalities = args.max_fatalities

#Remove trailing "/", if any
if options["output_directory"][-1] == "/":
    options["output_directory"] = options["output_directory"][:-1]

output_directory_base_path = Path(options["output_directory"])
if output_directory_base_path.exists():
    if output_directory_base_path.is_dir():
        print(f'{options["output_directory"]} is a directory; its contents will be overwritten. Continue (y/n)? ',
              end = "", flush = True)
        choice = input().lower()
        if (choice != "y"):
            print("Not continuing as directed.  Please provide another output directory.", flush = True)
            sys.exit(1)
    else:
        print(f'{options["output_directory"]} is a non-directory and already exists.', flush = True)
        print(f"Exiting. Please provide another output directory", flush = True)
        sys.exit(1)
else:
    output_directory_base_path.mkdir()

output_base = options["output_directory"]

csv_file_name = Path(output_base, "beta_fit_data.csv")

log_file_name = Path(output_base, "beta_fit_log.log")
logging.basicConfig(filename = log_file_name, level = logging.INFO)
print(f"check the log file {log_file_name} for updates", flush = True)

lambda_params = ["lambda_H", "lambda_W", "lambda_C", "lambda_T"]

# Field names for the output CSV file
field_names = ["run_index", "BETA_H", "BETA_W", "BETA_S", "BETA_C", "r2_score", "fit_growth_rate"] + lambda_params


def evaluate_betas(options, plot_file, min_fatalities, max_fatalities):
    fatality_file = Path(options["output_directory"],
                         "num_fatalities.csv")
    beta_data = dict()
    
    data = pd.read_csv(fatality_file).values
    num_data_points = data.shape[0]
    min_index = 0
    max_index = num_data_points - 1

    for i in range(0, num_data_points):
        val = data[i, 1]
        if val <= min_fatalities:
            min_index = i
        if val >= max_fatalities:
            max_index = i
            break;

    #The reshapes are to convert to 2D arrays
    times = data[min_index:(max_index + 1),0].reshape(-1, 1)
    fatalities = data[min_index:(max_index + 1),1].reshape(-1, 1)

    log_fatalities = np.log(fatalities)
    
    #Perform linear regression
    reg = LinearRegression().fit(times, log_fatalities)

    predictions = reg.predict(times)
    r2_score = metrics.r2_score(log_fatalities, predictions)
    daily_growth_rate = np.exp(reg.coef_[0,0])

    beta_data['r2_score'] = r2_score
    beta_data['daily_growth_rate'] = daily_growth_rate
    
    plt.plot(data[:,0], data[:,1], label="Model data")
    plt.plot(times, np.exp(predictions), label="Fit line")
    plt.xlabel("Time")
    plt.ylabel("Total fatalities")
    plt.title(f"BETA_H = {options['BETA_H']}\n"
              f"BETA_W = {options['BETA_W']}\n"
              f"BETA_S = {options['BETA_S']}\n"
              f"BETA_C = {options['BETA_C']}\n"
              f"Fit daily rate of growth = {daily_growth_rate:.4f}\n"
              f"R^2 = {r2_score}:.4f")
    plt.legend()

    plt.savefig(plot_file)
    logging.info(f"Saved plot to {plot_file}.")

    #Collect the cumulative mean lambda fraction data
    for name in lambda_params:
        lambda_csv_file_path = Path(options["output_directory"],
                                    f"cumulative_mean_fraction_{name}.csv")
        beta_data[name] = pd.read_csv(lambda_csv_file_path).values[-1, 1]
    
    return beta_data

#Run the simlator throughout the grid
run_index = 0

with open(csv_file_name, "w") as csv_file:
    
    csv_writer = csv.DictWriter(csv_file, fieldnames = field_names)

    csv_writer.writeheader()
    
    for BETA_H in BETA_H_VALUES:
        for BETA_W  in BETA_W_VALUES:
            for BETA_S in BETA_S_VALUES:
                for BETA_C in BETA_C_VALUES:
                    run_index += 1
                    
                    options["BETA_H"] = BETA_H
                    options["BETA_W"] = BETA_W
                    options["BETA_S"] = BETA_S
                    options["BETA_C"] = BETA_C                
                    
                    output_directory = output_base + f"/run_{run_index:07d}"
                    options["output_directory"] = output_directory
                    plot_file = Path(output_base, f"plot_for_run_index_{run_index:07d}.png")

                    output_directory_path = Path(output_directory)
                    if not output_directory_path.exists():
                        output_directory_path.mkdir()
                    
                    command = ["./drive_simulator"] + [f"--{key}={value}" for (key, value) in options.items()]
                    
                    logging.info(f"Run number = {run_index}\n"
                          f"BETA_H = {BETA_H}\n"
                          f"BETA_W = {BETA_W}\n"
                          f"BETA_S = {BETA_S}\n"
                          f"BETA_C = {BETA_S}\n"
                    )
                    logging.info("Command to be run\n" + " ".join(command))

                    try:
                        result = subprocess.run(command)
                        if result.returncode != 0:
                            raise Exception("simulator returned with error")
                        
                        beta_data = evaluate_betas(options, plot_file, min_fatalities, max_fatalities)
                        
                    except Exception as e:
                        print(f"error: run {run_index} terminated with error. Not using its data.\n"
                              f"error: see the log file {log_file_name}")
                        logging.error(f"error: run {run_index} terminated with error. Not using its data.")
                        logging.error("description of error follows")
                        logging.error(e)
                        continue

                    row = {
                        'run_index': run_index,
                        "BETA_H": BETA_H,
                        "BETA_W": BETA_W,
                        "BETA_S": BETA_S,
                        "BETA_C": BETA_C,
                        "r2_score": beta_data['r2_score'],
                        "fit_growth_rate": beta_data['daily_growth_rate']
                    }

                    for name in lambda_params:
                        row[name] = beta_data[name]

                    csv_writer.writerow(row)
