#!/usr/bin/env python
# coding: utf-8

# In[ ]:


from pathlib import Path
import math
import pandas as pd
import numpy as np
import os
import subprocess
import joblib
from collections import defaultdict
import functools
import datetime
import logging
from functools import wraps
from time import time
import argparse
import sys
import json

DEBUG=False

def measure(func):
    @wraps(func)
    def _time_it(*args, **kwargs):
        start = time()
        try:
            return func(*args, **kwargs)
        finally:
            end = int((time() - start)*1000)
            print(f"Finished in  {end} ms")
    return _time_it


# In[ ]:

# Some global variables that will be used
smaller_networks_scale = 9.0
params = {}
betas = {}
logfile = None

def processParams(params_json):
    global params
    global betas
    global smaller_networks_scale
    with open(params_json) as f:
        tmp_params = json.load(f)
        for k in ['H', 'W', 'C', 'TRAVEL']:
            betas[k] = tmp_params['betas'][k]
        del tmp_params['betas']
        params = tmp_params

        #Now to process the betas
        betas['S'] = 2 * betas['W']
        betas['PROJECT'] = betas['W'] * smaller_networks_scale
        betas['CLASS'] = betas['S'] * smaller_networks_scale
        betas['NBR_CELLS'] = betas['C'] * smaller_networks_scale
        betas['RANDOM_COMMUNITY'] = betas['NBR_CELLS']


def get_mean_fatalities(outputdir, nruns):
    data_dir = Path(outputdir)
    glob_str = f"run_[0-{nruns-1}]/num_fatalities.csv"
    files_list = data_dir.glob(glob_str)
    df = (pd.concat([pd.read_csv(f) for f in files_list], ignore_index = True)
          .groupby('Time').mean())
    return df

def get_mean_lambdas(outputdir, nruns):
    data_dir = Path(outputdir)
    lambdas = {'H': ["lambda_H"], 
               'W': ["lambda_W", "lambda_PROJECT"], 
               'C': ["lambda_C", "lambda_NBR_CELL", "lambda_RANDOM_COMMUNITY"]
              }
    values = {}
    for lam in lambdas.keys():
        lam_sum = 0
        for lam_inner in lambdas[lam]:
            glob_str = f"run_[0-{nruns-1}]/cumulative_mean_fraction_{lam_inner}.csv"
            files_list = data_dir.glob(glob_str)
            df = (pd.concat([pd.read_csv(f) for f in files_list], ignore_index = True)
              .groupby('Time').mean())
            lam_sum += df[f"cumulative_mean_fraction_{lam_inner}"].iloc[-1]
        values[lam] = lam_sum
    return values

def print_and_log(outstring, filename=logfile):
    print(outstring)
    with open(filename, "a+") as f:
        f.write(outstring)
        f.write("\n")

def print_betas():
    print("")
    print_and_log(f"BETA_H       : {betas['H']:.5f}", logfile)
    print_and_log(f"BETA_C       : {betas['C']:.5f}", logfile)
    print_and_log(f"BETA_W       : {betas['W']:.5f}", logfile)
    print_and_log(f"BETA_S       : {betas['S']:.5f}", logfile)
    print("")


# In[ ]:


def run_sim(run, params, betas):
    output_folder = Path(output_base, f"run_{run}")
    output_folder.mkdir(parents = True, exist_ok = True)
    cmd = [f"{cpp_exec}"]
    for param in params.keys():
        if params[param]!= False:
            if params[param]==True:
                cmd+= [f"--{param}"]
            else:
                cmd+= [f"--{param}", f"{params[param]}"]
    for b in betas.keys():
        cmd+= [f"--BETA_{b}",f"{betas[b]}"]
    cmd += [f"--input_directory", f"{input_folder}"]
    cmd += [f"--output_directory", f"{output_folder}"]
    print(" ".join(cmd))
    logging.info(" ".join(cmd))
    if DEBUG: 
        subprocess.call(cmd)
    else:
        ## Suppress other output
        subprocess.call(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)        


# In[ ]:


target_slope = 0.1803300052477795

def getTargetSlope():
    ## Don't need to run this every time.
    ## So putting it in a function that is never called
    global target_slope
    
    ECDP = pd.read_csv('data/ecdp.csv')
    india_data = ECDP[ECDP['geoId']=='IN'][['dateRep', 'deaths']][::-1]
    india_data["cumulative_deaths"] = india_data['deaths'].cumsum()
    india_data = india_data[(india_data['cumulative_deaths'] > 10) 
                            & (india_data['cumulative_deaths'] < 200)].reset_index(drop=True)
    target_slope = np.polyfit(india_data.index, np.log(india_data['cumulative_deaths']), deg = 1)[0]

# In[ ]:


def get_slope(outputdir, nruns, low_thresh = 10, up_thresh = 200):
    df = get_mean_fatalities(outputdir, nruns)
    df = df[(df['num_fatalities'] > low_thresh)]
    if df.shape[0] < 5:
        raise TypeError("Too few fatalities")
    else:
        df = df[df['num_fatalities'] < up_thresh]
        return np.polyfit(df.index, np.log(df['num_fatalities']), deg = 1)[0]


# In[ ]:


def update_betas(diffs, count=0):
    global smaller_networks_scale
    
    if diffs == -1: #Happens if the getslope fails due to too few fatalities
        print_betas()
        print("Too few fatalities. Doubling betas")
        betas['H'] *= 2
        betas['W'] *= 2
        betas['C'] *= 2    
    else:
        (lambda_H_diff, lambda_W_diff, lambda_C_diff, slope_diff) = diffs

        step_beta_H = -1*lambda_H_diff/(3+count) 
        step_beta_W = -1*lambda_W_diff/(3+count) 
        step_beta_C = -1*lambda_C_diff/(3+count) 
        beta_scale_factor = max(min(np.exp(slope_diff),1.5), 0.66)

        if (count>=30):
            beta_scale_factor = max(min(np.exp(slope_diff/(count-25)),1.5), 0.66)
        elif (abs(lambda_H_diff)<0.02 and abs(lambda_W_diff)<0.02 and abs(lambda_C_diff)<0.02):
            beta_scale_factor = max(min(np.exp(slope_diff/(5)),1.5), 0.66)

        betas['H'] = max(betas['H'] + step_beta_H , 0) * beta_scale_factor
        betas['W'] = max(betas['W'] + step_beta_W , 0) * beta_scale_factor
        betas['C'] = max(betas['C'] + step_beta_C , 0) * beta_scale_factor
    
    betas['S'] = betas['W'] * 2
    betas['CLASS'] = betas['S'] * smaller_networks_scale
    betas['PROJECT'] = betas['W'] * smaller_networks_scale
    betas['RANDOM_COMMUNITY']  =  betas['C'] * smaller_networks_scale
    betas['NBR_CELLS'] = betas['C'] * smaller_networks_scale

def satisfied(diffs, slope_tolerance = 0.001, lam_tolerance = 0.01):
    if diffs==-1:
        return False
    else:
        return (diffs[0] < lam_tolerance and
            diffs[1] < lam_tolerance and
            diffs[2] < lam_tolerance and
            diffs[3] < slope_tolerance)        


# In[ ]:


def run_parallel(nruns, ncores, params, betas):
    processed_list = joblib.Parallel(n_jobs=ncores)(
        joblib.delayed(run_sim)(run, params, betas) for run in range(nruns)
    )     
    
@measure
def calibrate(nruns, ncores, params, betas, resolution=4):
    run_parallel(nruns, ncores, params, betas)    
    try:
        slope = get_slope(output_base, nruns)
    except TypeError:
        return -1

    lambdas = get_mean_lambdas(output_base, nruns)
    [lambda_H, lambda_W, lambda_C] = [lambdas[key] for key in ['H', 'W', 'C']]
    lambda_H_diff = float(lambda_H) - (1.0/3)
    lambda_W_diff = float(lambda_W) - (1.0/3)
    lambda_C_diff = float(lambda_C) - (1.0/3)
        
    slope_diff = target_slope - slope

    print_betas()
    print_and_log(f"lambda_H_diff: {lambda_H_diff:.5f}", logfile)
    print_and_log(f"lambda_W_diff: {lambda_W_diff:.5f}", logfile)
    print_and_log(f"lambda_C_diff: {lambda_C_diff:.5f}", logfile)
    print_and_log(f"slope_diff   : {slope_diff:.5f}", logfile)
    print_and_log("", logfile)
    logging.info(f"Slope: slope")
    return (lambda_H_diff, lambda_W_diff, lambda_C_diff, slope_diff)


# In[ ]:


def main():
    global betas
    global params
    global smaller_networks_scale
    global logfile
    global cpp_exec, output_base, input_folder

    
    resolution = 4

    default_binary_path = "../cpp-simulator/drive_simulator"
    default_input_folder = "../staticInst/data/mumbai_1million/"
    default_output_folder = "calibration_output/"
    default_nruns = 6

    class MyParser(argparse.ArgumentParser):
        def error(self, message):
            sys.stderr.write('error: %s\n' % message)
            sys.stderr.write('\n\n')
            self.print_help()
            sys.exit(2)
    
    my_parser = MyParser(description='Calibration directories',
                         formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    my_parser.add_argument(
        '-e', help='Location of drive_simulator binary',
        default=default_binary_path)
    my_parser.add_argument(
        '-i', help='Location of input folder (with city json files)',
        default=default_input_folder)
    my_parser.add_argument(
        '-o', help='Location of output folder',
        default=default_output_folder)
    my_parser.add_argument(
        '-s', help="scale factor for smaller network",
        default = smaller_networks_scale)
    my_parser.add_argument(
        '-r', help="number of runs per calibration step",
        default = default_nruns)
    my_parser.add_argument(
        '-c', help="Number of cpus to use",
        default = joblib.cpu_count())
    my_parser.add_argument(
        '-p', help='Starting parameters json',
        required=True
    )

    args = my_parser.parse_args() or my_parser.print_help()
    cpp_exec = f"./{args.e}" or exit("Error: Couldn't process argument to -e.\n", my_parser.print_help())
    input_folder = args.i or exit("Error: Couldn't process argument to -i.\n" , my_parser.print_help())
    output_base = args.o or exit("Error: Couldn't process argument to -o.\n", my_parser.print_help())
    params_json = args.p or exit("Error: Couldn't process argument to -p.\n" , my_parser.print_help())
    nruns = int(args.r) or exit("Error: Couldn't process argument to -r.\n", my_parser.print_help())
    ncores = int(args.c) or exit("Error: couldn't process argument to -c.\n", my_parser.print_help())

    smaller_networks_scale = float(args.s)

    Path(output_base).mkdir(parents=True, exist_ok = True)
    logfile = Path(output_base, "calibration.log")

    processParams(params_json)
    
    count = 1
    while True:
        print_and_log("", logfile)
        print_and_log(f"Count: {count}", logfile)
        diffs = calibrate(nruns, ncores, params, betas)
        if satisfied(diffs):
            print("Satisfied!")
            break
        else:
            update_betas(diffs, count)
            count+=1


# In[ ]:


if __name__ == "__main__":
    main()


# In[ ]:




