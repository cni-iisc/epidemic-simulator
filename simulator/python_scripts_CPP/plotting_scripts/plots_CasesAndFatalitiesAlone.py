# -*- coding: utf-8 -*-
#Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
#SPDX-License-Identifier: Apache-2.0
"""
Plot India, Bangalore and Mumbai cases and deaths.
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.optimize import least_squares

greys = sns.color_palette("Greys", 10)
blues = sns.color_palette("Blues", 10)

simStr = ['0','1','2','3','4']
intvStr = ['0', '1', '2', '3', '4', '5', '6']
colorStr = ['r', 'b', 'g', 'y', 'm', 'c', 'k', 'k--']
labelStr = ['No intervention', 'Lockdown', 'Lockdown 40 days', 'Lockdown 26 days', 'LD26-PE', 'LD26-PE-SC', 'LD26-PEOE']

lockdown_start = 25 # Read 25 as March 25

dIndia = pd.read_csv('../data/IndiaCases.csv')
dIndia_time = dIndia['Time'].values # index 1 is March 1st
dIndia_h = dIndia['num_cases'].values
dIndia_f = dIndia['num_fatalities'].values

dBangalore = pd.read_csv('../data/BangaloreCases.csv')
dBangalore_time = dBangalore['day'].values # Read 1 as March 1st
dBangalore_h = dBangalore['cumulative_cases'].values
dBangalore_f = dBangalore['cumulative_deaths'].values

dMumbai = pd.read_csv('../data/MumbaiCases.csv')
dMumbai_time = dMumbai['day'].values # Read 1 as March 1st
dMumbai_h = dMumbai['cumulative_cases'].values
dMumbai_f = dMumbai['cumulative_deaths'].values

#Plot hospitalised
plt.subplot(2,1,1)
plt.rcParams["figure.figsize"] = (12,9)
plt.xlim(right=70)
#plt.xlim(left=0)
plt.yscale('log')

plt.plot(dIndia_time,dIndia_h,'ko-', mfc='none', label='India cases')
plt.plot(dMumbai_time, dMumbai_h,'bo-', mfc='none', label='Mumbai cases')

plt.plot(dIndia_time, dIndia_f,'ko-', label='India fatalities')
plt.plot(dMumbai_time, dMumbai_f,'bo-', label='Mumbai fatalities')

plt.xticks([1,15,32,46,62],['March 1','March 15','April 1','April 15','May 1'])
plt.xlabel('Date',fontsize=10,labelpad=10)
plt.ylabel('Cumulative cases and fatalities')
#plt.grid(b='true',axis='both',color='k', linestyle='-', linewidth=1)
plt.legend(prop={'size':15})
plt.axvspan(lockdown_start,lockdown_start+26, alpha = 0.75, color=blues[2],zorder=-1)
plt.axvspan(lockdown_start+26,lockdown_start+40, alpha = 0.75, color=blues[1],zorder=-1)
plt.grid(b='true',axis='both',color='k',alpha=0.25, linestyle='-', linewidth=1)

#Plot hospitalised
plt.subplot(2,1,2)
#plt.rcParams["figure.figsize"] = (12,18)
plt.xlim(right=70)
#plt.xlim(left=0)
plt.yscale('log')

plt.plot(dIndia_time,dIndia_h,'ko-', mfc='none', label='India cases')
plt.plot(dBangalore_time, dBangalore_h,'ro-', mfc='none', label='Bengaluru cases')

plt.plot(dIndia_time, dIndia_f,'ko-', label='India fatalities')
plt.plot(dBangalore_time, dBangalore_f,'ro-', label='Bengaluru fatalities')

plt.xticks([1,15,32,46,62],['March 1','March 15','April 1','April 15','May 1'])
plt.xlabel('Date',fontsize=10,labelpad=10)
plt.ylabel('Cumulative cases and fatalities')
#plt.grid(b='true',axis='both',color='k', linestyle='-', linewidth=1)
plt.legend(prop={'size':15})
plt.axvspan(lockdown_start,lockdown_start+26, alpha = 0.75, color=blues[2],zorder=-1)
plt.axvspan(lockdown_start+26,lockdown_start+40, alpha = 0.75, color=blues[1],zorder=-1)
plt.grid(b='true',axis='both',color='k',alpha=0.25, linestyle='-', linewidth=1)

plt.show()
#x = [20, 30]
#y = [(11, 200), (8, 120)]
#plt.plot((x,x),([i for (i,j) in y], [j for (i,j) in y]),color='black')
plt.close()
