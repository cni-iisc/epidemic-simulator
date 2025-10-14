#Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
#SPDX-License-Identifier: Apache-2.0

# This code is used to tune the parameters of the simulator so as to match the probability of getting an infection (i.e. lambda_home,
# lambda_workplace and lambda_community) to certain target value (default target value is 1/3 each) as well as to match the number 
# of fatalities curve to actual data.


#from calculate_means_CPP import calculate_means
from calculate_r0 import calculate_r0
from calibrate import calibrate
from joblib import Parallel, delayed
import os
import numpy as np 
import pandas as pd
import time

NUM_DAYS=120
INCUBATION_PERIOD=2.3
MEAN_ASYMPTOMATIC_PERIOD=0.5
MEAN_SYMPTOMATIC_PERIOD=5
SYMPTOMATIC_FRACTION=0.67
MEAN_HOSPITAL_REGULAR_PERIOD=8
MEAN_HOSPITAL_CRITICAL_PERIOD=8
COMPLIANCE_PROBABILITY=0.9

##########################################################################################################
### IMPORTANT: Check if these value fit the city we want to calibrate on. Currently set to Mumbai values #
##########################################################################################################

F_KERNEL_A= 2.709
F_KERNEL_B= 1.278
BETA_H=0.91100
BETA_W=0.48803
BETA_C=0.22089
BETA_S=0.97607
BETA_TRAVEL=0


HD_AREA_FACTOR=2.0
HD_AREA_EXPONENT=0
INTERVENTION=0
output_directory_base="../../cpp-simulator/outputs/calibration_2020-05-31/"
input_directory="../../staticInst/data/mumbai_1million_2020-05-31"
CALIBRATION_DELAY=0
DAYS_BEFORE_LOCKDOWN=0
# Set this to "--SEED_HD_AREA_POPULATION" to seed hd area population
# as well.
# SEED_HD_AREA_POPULATION="--SEED_HD_AREA_POPULATION"
SEED_HD_AREA_POPULATION=" "
# Set this to "--SEED_ONLY_NON_COMMUTER" to seed only those who do not
# take public transit
# SEED_ONLY_NON_COMMUTER="--SEED_ONLY_NON_COMMUTER"
SEED_ONLY_NON_COMMUTER=" "
# Set this to "--SEED_FIXED_NUMBER" to seed only a fixed number of
# people. In this case, the value of INIT_FRAC_INFECTED will be
# ignored in favour of the value of INIT_FIXED_NUMBER_INFECTED
SEED_FIXED_NUMBER="--SEED_FIXED_NUMBER"
#SEED_FIXED_NUMBER=" "
INIT_FIXED_NUMBER_INFECTED=100
INTERVENTION=0
EXEC_DIR = "./../../cpp-simulator"
LOGFILE = output_directory_base + "/calibration.log"
######################
def calculate_means_fatalities_CPP(output_directory_base, num_sims,results_dir):
    column_names = ['timestep','dead']
    master_df = pd.DataFrame(columns=column_names)
    for sim_count in range(num_sims):
        output_directory=output_directory_base+"/intervention_"+ str(INTERVENTION)+"_"+str(sim_count)
        print(output_directory)
        df_temp = pd.read_csv(output_directory+"/num_fatalities.csv")
        df_temp.columns = column_names
        master_df = master_df.append(df_temp)
        print(master_df.size)
    master_df[column_names] = master_df[column_names].apply(pd.to_numeric)
    df_mean = master_df.groupby(['timestep']).mean()
    df_mean.to_csv(results_dir+'dead_mean.csv')
    print(df_mean)

def calculate_means_lambda_CPP(output_directory_base, num_sims,results_dir):
    column_names_lambda_H = ['timestep','lambda H']
    column_names_lambda_W = ['timestep','lambda W']
    column_names_lambda_C = ['timestep','lambda C']
    column_names = [column_names_lambda_H, column_names_lambda_W, column_names_lambda_C]
    lambda_array=['lambda_H','lambda_W','lambda_C']
    for lambda_count,lambda_ in enumerate(lambda_array):
        master_df = pd.DataFrame(columns=column_names[lambda_count])
        val = column_names[lambda_count][1]
        for sim_count in range(num_sims):
            output_directory=output_directory_base+"/""intervention_"+ str(INTERVENTION)+"_"+str(sim_count)
            df_temp = pd.read_csv(output_directory+"/cumulative_mean_fraction_"+lambda_+".csv")
            df_temp.columns = column_names[lambda_count]
            master_df = master_df.append(df_temp)
        master_df[column_names[lambda_count]] = master_df[column_names[lambda_count]].apply(pd.to_numeric)
        df_mean = master_df.groupby(['timestep']).mean()
        df_mean.to_csv(results_dir+val+'_mean.csv')

def run_sim(num_sims_count, params):
    print("Internal loop. Loop count = ", num_sims_count)
    output_directory=params['outputDirectoryBase']+"/""intervention_"+ str(params['intervention'])+"_"+str(num_sims_count)

    os.system("mkdir -p "+output_directory)

    command="time"+ " "
    command+=params['execDir']+"/drive_simulator"+ " "
    command+=params['seedHDAreaPopulation'] + " " 
    command+=params['seedOnlyNonCommuter'] + " "
    command+=params['seedFixedNumber']
    command+=" --NUM_DAYS "+ str(params['numDays'])
    command+=" --INIT_FIXED_NUMBER_INFECTED "+ str(params['initFixedNumberInfected'])
    command+=" --INCUBATION_PERIOD " +  str(params['incubationPeriod'])
    command+=" --MEAN_ASYMPTOMATIC_PERIOD " +  str(params['MeanAsymptomaticPeriod']) 
    command+=" --MEAN_SYMPTOMATIC_PERIOD " + str(params['MeanSymptomaticPeriod']) 
    command+=" --SYMPTOMATIC_FRACTION " +  str(params['symptomaticFraction']) 
    command+=" --MEAN_HOSPITAL_REGULAR_PERIOD " + str(params['meanHospitalRegularPeriod'])
    command+=" --MEAN_HOSPITAL_CRITICAL_PERIOD " + str(params['meanHospitalCriticalPeriod'])
    command+=" --COMPLIANCE_PROBABILITY " + str(params['complianceProbability'])
    command+=" --F_KERNEL_A " + str(params['FKernelA'])
    command+=" --F_KERNEL_B " + str(params['FKernelB'])
    command+=" --BETA_H " + str(params['betaH'])
    command+=" --BETA_W " + str(params['betaW'])
    command+=" --BETA_C " + str(params['betaC'])
    command+=" --BETA_S " + str(params['betaS'])
    command+=" --BETA_TRAVEL " + str(params['betaTravel'])
    command+=" --HD_AREA_FACTOR " + str(params['hdAreaFactor'])
    command+=" --HD_AREA_EXPONENT " + str(params['hdAreaExponent'])
    command+=" --INTERVENTION " + str(params['intervention'])
    command+=" --IGNORE_ATTENDANCE_FILE "
    command+=" --output_directory " + str(output_directory)
    command+=" --input_directory " + str(params['inputDirectory'])
    command+=" --CALIBRATION_DELAY " + str(params['calibrationDelay'])
    command+=" --DAYS_BEFORE_LOCKDOWN " + str(params['daysBeforeLockdown'])

    print(command)

    os.system(command)

    return (True)

###########################
continue_run = True
resolution = 4
num_sims = 6 #cpu_count()/2
count = 0
num_cores = 12 #cpu_count()

print ('Cpu count: ', num_cores)

while (continue_run):
   
    params = { 'execDir': EXEC_DIR,'seedHDAreaPopulation': SEED_HD_AREA_POPULATION, 'seedOnlyNonCommuter': SEED_ONLY_NON_COMMUTER,
               'seedFixedNumber':SEED_FIXED_NUMBER, 'seedFixedNumber':SEED_FIXED_NUMBER, 'numDays': NUM_DAYS, 
               'initFixedNumberInfected': INIT_FIXED_NUMBER_INFECTED, 'incubationPeriod': INCUBATION_PERIOD, 
               'MeanAsymptomaticPeriod': MEAN_ASYMPTOMATIC_PERIOD, 'MeanSymptomaticPeriod': MEAN_SYMPTOMATIC_PERIOD, 'symptomaticFraction': SYMPTOMATIC_FRACTION, 
               'meanHospitalRegularPeriod': MEAN_HOSPITAL_REGULAR_PERIOD, 'meanHospitalCriticalPeriod': MEAN_HOSPITAL_CRITICAL_PERIOD, 
               'complianceProbability': COMPLIANCE_PROBABILITY, 'FKernelA': F_KERNEL_A, 'FKernelB': F_KERNEL_B, 
               'betaH': BETA_H, 'betaW': BETA_W, 'betaC': BETA_C, 'betaS': BETA_S, 'betaTravel': BETA_TRAVEL,
               'hdAreaFactor':HD_AREA_FACTOR, 'hdAreaExponent':HD_AREA_EXPONENT, 'intervention': INTERVENTION, 
               'outputDirectoryBase': output_directory_base, 'inputDirectory': input_directory,
               'calibrationDelay': CALIBRATION_DELAY, 'daysBeforeLockdown': DAYS_BEFORE_LOCKDOWN }
    
    print ('Parameters: ', params)    
    
    start_time = time.time()
    processed_list = Parallel(n_jobs=num_cores)(delayed(run_sim)(simNum, params) for simNum in range(num_sims))     
    print ('Execution time: ',time.time()-start_time, ' seconds') 

    ##############################################################
    calculate_means_fatalities_CPP(output_directory_base, num_sims,"./data/")
    calculate_means_lambda_CPP(output_directory_base, num_sims,"./data/") 
    
    [flag, BETA_SCALE_FACTOR, step_beta_h, step_beta_w, step_beta_c, delay, slope_diff, lambda_h_diff, lambda_w_diff, lambda_c_diff] = calibrate(resolution,count)
    with open(LOGFILE, "a+") as logfile:
        logfile.write(f"beta_h: {BETA_H}\n")
        logfile.write(f"beta_w: {BETA_W}\n")
        logfile.write(f"beta_c: {BETA_C}\n")
        logfile.write(f"beta_s: {BETA_S}\n")
        logfile.write(f"slope_diff: {slope_diff}\n")
        logfile.write(f"lambda_h_diff: {lambda_h_diff}\n")
        logfile.write(f"lambda_w_diff: {lambda_w_diff}\n")
        logfile.write(f"lambda_c_diff: {lambda_c_diff}\n\n\n")        
    count+=1    
    if flag == True:
        continue_run = False
    else:
        BETA_H = max(BETA_H + step_beta_h,0)*BETA_SCALE_FACTOR
        BETA_W = max(BETA_W + step_beta_w,0)*BETA_SCALE_FACTOR
        BETA_S = BETA_W * 2
        BETA_C = max(BETA_C + step_beta_c,0)*BETA_SCALE_FACTOR
        #INIT_FRAC_SCALE_FACTOR = INIT_FRAC_SCALE_FACTOR*init_frac_mult_factor
        print("\n\n\n")
        print ("count:", count, '. BETA_H: ', BETA_H, '. BETA_W: ',BETA_W, '. BETA_S: ', BETA_S, '. BETA_C: ', BETA_C, 'Delay: ', delay )
    #continue_run = False

