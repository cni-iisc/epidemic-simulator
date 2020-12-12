# -*- coding: utf-8 -*-
#Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
#SPDX-License-Identifier: Apache-2.0
"""
Plot the simulator output and compare with India data.
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.optimize import least_squares

#Specify the directory where the simulator output folders for different interventions and multiple runs for given intervention are present.
#The current simulator expects output folders be of the form intervention_x_id_y, where x is the intervention index and y is the run index
baseFileStr = ''#'/simulator_output_directory/'
intvStr = ['0', '1', '2', '3', '4', '5', '6'] # output folder tag for interventions. Length = number of interventions
simStr = ['0','1','2','3','4']  # output folder tag for run index.

colorStr = ['r', 'b', 'g', 'y', 'm', 'c', 'k', 'k--']
labelStr = ['No intervention', 'Lockdown', 'LD40', 'LD26', 'LD26-PE-SC', 'LD26-PE', 'LD26-PEOE']


#baseFileStr2 = '16042020/bangalore/bangalore-2-10M-sims-parallel'

dFhospitalised = []
dFfatalities = []
dFhospitalBeds = []
dFcriticalBeds = []
dFTime = []

hospitalised = []
fatalities = []
hospitalBeds = []
criticalBeds = []

hospitalisedStd = []
fatalitiesStd = []
hospitalBedsStd = []
criticalBedsStd = []


for intvIdx in range(0,len(intvStr)):
    
    dFhospitalised_local = []
    dFfatalities_local = []
    dFhospitalBeds_local = []
    dFcriticalBeds_local = []

    for simIdx in range(0,len(simStr)):
        
        if (simIdx == 0 and intvIdx ==0):
            dFTime = pd.read_csv(baseFileStr + '/intervention_' + intvStr[intvIdx] \
                + '_id_' + simStr[simIdx] + '/num_cumulative_hospitalizations.csv')['Time'].values
                                 
        # read first set
        dFhospitalised_local.append(pd.read_csv(baseFileStr + '/intervention_' + intvStr[intvIdx] \
                + '_id_' + simStr[simIdx] + '/num_cumulative_hospitalizations.csv')['num_cumulative_hospitalizations'].values.tolist())
        dFfatalities_local.append(pd.read_csv(baseFileStr + '/intervention_' + intvStr[intvIdx] \
                                                           + '_id_' + simStr[simIdx] + '/num_fatalities.csv')['num_fatalities'].values.tolist())
        dFhospitalBeds_local.append(pd.read_csv(baseFileStr + '/intervention_' + intvStr[intvIdx] \
                + '_id_' + simStr[simIdx] +  '/num_hospitalised.csv')['num_hospitalised'].values.tolist())
        dFcriticalBeds_local.append(pd.read_csv(baseFileStr + '/intervention_' + intvStr[intvIdx] \
                + '_id_' + simStr[simIdx] + '/num_critical.csv')['num_critical'].values.tolist())
        
#                # read first set
#        dFhospitalised_local.append(pd.read_csv(baseFileStr2 + '/intervention_' + intvStr[intvIdx] \
#                + '_id_' + simStr[simIdx] + '/num_cumulative_hospitalizations.csv')['num_cumulative_hospitalizations'].values.tolist())
#        dFfatalities_local.append(pd.read_csv(baseFileStr2 + '/intervention_' + intvStr[intvIdx] \
#                                                           + '_id_' + simStr[simIdx] + '/num_fatalities.csv')['num_fatalities'].values.tolist())
#        dFhospitalBeds_local.append(pd.read_csv(baseFileStr2 + '/intervention_' + intvStr[intvIdx] \
#                + '_id_' + simStr[simIdx] +  '/num_hospitalised.csv')['num_hospitalised'].values.tolist())
#        dFcriticalBeds_local.append(pd.read_csv(baseFileStr2 + '/intervention_' + intvStr[intvIdx] \
#                + '_id_' + simStr[simIdx] + '/num_critical.csv')['num_critical'].values.tolist())

        
    dFhospitalised = np.array(dFhospitalised_local) 
    dFfatalities = np.array(dFfatalities_local)
    dFhospitalBeds = np.array(dFhospitalBeds_local)
    dFcriticalBeds = np.array(dFcriticalBeds_local)
    
    hospitalisedAve = np.average(dFhospitalised, axis=0)
    fatalitiesAve = np.average(dFfatalities, axis=0)
    hospitalBedsAve = np.average(dFhospitalBeds, axis=0)
    criticalBedsAve  = np.average(dFcriticalBeds, axis=0)
    
    hospitalisedStd_loc = np.std(dFhospitalised, axis=0)
    fatalitiesStd_loc = np.std(dFfatalities, axis=0)
    hospitalBedsStd_loc = np.std(dFhospitalBeds, axis=0)
    criticalBedsStd_loc  = np.std(dFcriticalBeds, axis=0)

                
    hospitalised.append(hospitalisedAve)
    fatalities.append(fatalitiesAve)
    hospitalBeds.append(hospitalBedsAve)
    criticalBeds.append(criticalBedsAve)
    
    hospitalisedStd.append(hospitalisedStd_loc)
    fatalitiesStd.append(fatalitiesStd_loc)
    hospitalBedsStd.append(hospitalBedsStd_loc)
    criticalBedsStd.append(criticalBedsStd_loc)

greys = sns.color_palette("Greys", 10)
blues = sns.color_palette("Blues", 10)

offset = 23
time0 = dFTime - offset ## Read 1 as March 1st
lockdown_start = 25 # Read 25 as March 25th
hospitalisation_delay = 0

dIndia = pd.read_csv('../data/IndiaCases.csv')
dIndia_time = dIndia['Time'].values # index 1 is March 1st
dIndia_h = dIndia['num_cases'].values
dIndia_f = dIndia['num_fatalities'].values

#dBangalore = pd.read_csv('../data/BangaloreCases.csv')
#dBangalore_time = dBangalore['day'].values # Read 1 as March 1st
#dBangalore_h = dBangalore['cumulative_cases'].values
#dBangalore_f = dBangalore['cumulative_deaths'].values

#Plot hospitalised
plt.rcParams["figure.figsize"] = (12,9)
plt.xlim(right=150-offset)
#plt.xlim(left=0)
plt.yscale('log')

for intvIdx in range (0,len(intvStr)):
    plt.plot(time0+hospitalisation_delay,hospitalised[intvIdx],colorStr[intvIdx],label=labelStr[intvIdx])
    plt.fill_between(time0, hospitalised[intvIdx]-hospitalisedStd[intvIdx], hospitalised[intvIdx]+hospitalisedStd[intvIdx], color=colorStr[intvIdx],alpha=0.1)

plt.plot(dIndia_time,dIndia_h,'ko-', label='India cases')

plt.xticks([1,15,32,46,62,76,93,107,123],['March 1','March 15','April 1','April 15','May 1', 'May 15','June 1','June 15','July 1'])
plt.xlabel('Date',fontsize=15,labelpad=20)
plt.ylabel('Cumulative hospitalised cases',fontsize=15)
plt.grid(b='true',axis='both',color='k',alpha=0.25, linestyle='-', linewidth=1)
plt.legend(prop={'size':15})
plt.axvspan(lockdown_start,lockdown_start+26, alpha = 0.75, color=blues[2],zorder=-1)
plt.axvspan(lockdown_start+26,lockdown_start+40, alpha = 0.75, color=blues[1],zorder=-1)
plt.show()

plt.close()


#Plot fatalities
plt.rcParams["figure.figsize"] = (12,9)
plt.xlim(right=150-offset)
#plt.xlim(left=0)
plt.yscale('log')
fill_start_index = 0
for intvIdx in range (0,len(intvStr)):
    plt.plot(time0,fatalities[intvIdx],colorStr[intvIdx],label=labelStr[intvIdx])
    plt.fill_between(time0[fill_start_index:], fatalities[intvIdx][fill_start_index:]-fatalitiesStd[intvIdx][fill_start_index:], fatalities[intvIdx][fill_start_index:]+fatalitiesStd[intvIdx][fill_start_index:], color=colorStr[intvIdx],alpha=0.1)

plt.plot(dIndia_time, dIndia_f,'ko-', label='India fatalities')
plt.xticks([1,15,32,46,62,76,93,107,123],['March 1','March 15','April 1','April 15','May 1', 'May 15','June 1','June 15','July 1'])

plt.xlabel('Date',fontsize=15,labelpad=20)
plt.ylabel('Fatalities',fontsize=15)
plt.grid(b='true',axis='both',color='k',alpha=0.25, linestyle='-', linewidth=1)
plt.legend(prop={'size':15})
plt.axvspan(lockdown_start,lockdown_start+26, alpha = 0.75, color=blues[2],zorder=-1)
plt.axvspan(lockdown_start+26,lockdown_start+40, alpha = 0.75, color=blues[1],zorder=-1)
plt.show()

plt.close()


#Plot fatalities, linear scale
plt.rcParams["figure.figsize"] = (12,9)
plt.xlim(right=150-offset)
#plt.xlim(left=0)
#plt.yscale('log')

for intvIdx in range (0,len(intvStr)):
    plt.plot(time0,fatalities[intvIdx],colorStr[intvIdx],label=labelStr[intvIdx])
    plt.fill_between(time0, fatalities[intvIdx]-fatalitiesStd[intvIdx], fatalities[intvIdx]+fatalitiesStd[intvIdx], color=colorStr[intvIdx],alpha=0.1)

plt.plot(dIndia_time,dIndia_f,'ko-', label='India fatalities')

plt.xticks([1,15,32,46,62,76,93,107,123],['March 1','March 15','April 1','April 15','May 1', 'May 15','June 1','June 15','July 1'])
plt.xlabel('Date',fontsize=15,labelpad=20)
plt.ylabel('Fatalities ',fontsize=15)
plt.grid(b='true',axis='both',color='k',alpha=0.25, linestyle='-', linewidth=1)
plt.legend(prop={'size':15})
plt.axvspan(lockdown_start,lockdown_start+26, alpha = 0.75, color=blues[2],zorder=-1)
plt.axvspan(lockdown_start+26,lockdown_start+40, alpha = 0.75, color=blues[1],zorder=-1)
plt.show()

plt.close()


#Plot hospitalised daily
plt.rcParams["figure.figsize"] = (12,9)
plt.xlim(right=150-offset)
#plt.xlim(left=0)
plt.yscale('log')

for intvIdx in range (0,len(intvStr)):
    plt.plot(time0,hospitalBeds[intvIdx],colorStr[intvIdx],label=labelStr[intvIdx])
    plt.fill_between(time0, hospitalBeds[intvIdx]-hospitalBedsStd[intvIdx], hospitalBeds[intvIdx]+criticalBedsStd[intvIdx], color=colorStr[intvIdx],alpha=0.1)


plt.xticks([1,15,32,46,62,76,93,107,123],['March 1','March 15','April 1','April 15','May 1', 'May 15','June 1','June 15','July 1'])
plt.xlabel('Date',fontsize=15,labelpad=20)
plt.ylabel('Number of hospital beds (daily)',fontsize=15)
plt.grid(b='true',axis='both',color='k',alpha=0.25, linestyle='-', linewidth=1)
plt.legend(prop={'size':15})
plt.axvspan(lockdown_start,lockdown_start+26, alpha = 0.75, color=blues[2],zorder=-1)
plt.axvspan(lockdown_start+26,lockdown_start+40, alpha = 0.75, color=blues[1],zorder=-1)
plt.show()

plt.close()


#Plot critical daily
plt.rcParams["figure.figsize"] = (12,9)
plt.xlim(right=150-offset)
#plt.xlim(left=0)
plt.yscale('log')
plt.axvspan(lockdown_start,lockdown_start+26, alpha = 0.75, color=blues[2],zorder=-1)
plt.axvspan(lockdown_start+26,lockdown_start+40, alpha = 0.75, color=blues[1],zorder=-1)
for intvIdx in range (0,len(intvStr)):
    plt.plot(time0,criticalBeds[intvIdx],colorStr[intvIdx],label=labelStr[intvIdx])
    plt.fill_between(time0[fill_start_index:], criticalBeds[intvIdx][fill_start_index:]-criticalBedsStd[intvIdx][fill_start_index:], criticalBeds[intvIdx][fill_start_index:]+criticalBedsStd[intvIdx][fill_start_index:], color=colorStr[intvIdx],alpha=0.1)
    


plt.xticks([1,15,32,46,62,76,93,107,123],['March 1','March 15','April 1','April 15','May 1', 'May 15','June 1','June 15','July 1'])
plt.xlabel('Date',fontsize=15,labelpad=20)
plt.ylabel('Number of critical beds (daily)',fontsize=15)
plt.grid(b='true',axis='both',color='k',alpha=0.25, linestyle='-', linewidth=1)
plt.legend(prop={'size':15})
plt.show()

plt.close()

