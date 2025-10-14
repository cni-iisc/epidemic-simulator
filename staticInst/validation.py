#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#"""
#Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
#SPDX-License-Identifier: Apache-2.0
#"""
__name__ = "Script for validating city files"


import pandas as pd
import numpy as np
import math
import json
import matplotlib.pyplot as plt
import os

def compute_age_distribution(ageDistribution):
    age_values = np.arange(0,81,1)
    age_distribution_over_gap5 = ageDistribution

    age_distribution = []
    for i in range(0,len(age_distribution_over_gap5)-1):
        for j in range(0,5):
            age_distribution.append(age_distribution_over_gap5[i]/5)
    age_distribution.append(age_distribution_over_gap5[16])
    age_distribution = np.array(age_distribution)
    age_distribution = age_distribution/sum(age_distribution)
    return age_values, age_distribution

def compute_household_size_distribution(householdSizes, householdDistribution):
    household_sizes = [] 
    household_dist = []
    for i in range(len(householdSizes)):
        size = householdSizes[i]
        if "-" in size:
            size = size.split('-')
            diff = int(size[1]) - int(size[0]) + 1 #inclusive difference where you want the first number to be present
            for j in range(diff):
                household_sizes.append(int(size[0])+j)
                household_dist.append(householdDistribution[i]/diff)
        elif "+" in size: #last index
            size = int(size.split("+")[0])
            household_sizes.append(size)
            household_dist.append(householdDistribution[i])
        else:
            household_sizes.append(int(householdSizes[i]))
            household_dist.append(householdDistribution[i])

    household_distribution = np.array(household_dist)/np.sum(household_dist)
    return household_sizes, household_distribution

def extrapolate_school_size_distribution(schoolDistribution,avgSchoolsize):
    # generate schools size distribution
    schoolsize_values = np.arange(50,901,1)
    schoolsize_distribution_over_gap100 = schoolDistribution # 50-99, 100-199, ..., 800 - 899, 900+
    schoolsize_distribution = []
    for i in range(1,len(schoolsize_distribution_over_gap100)-1):
        for j in range(0,100):
            schoolsize_distribution.append(schoolsize_distribution_over_gap100[i]/100)

    for i in range(0,50):
        schoolsize_distribution.insert(0,schoolsize_distribution_over_gap100[0]/50)

    schoolsize_distribution.append(schoolsize_distribution_over_gap100[len(schoolsize_distribution_over_gap100)-1])
    schoolsize_distribution = np.array(schoolsize_distribution)
    schoolsize_distribution = schoolsize_distribution/np.sum(schoolsize_distribution)
    mean_school_size = np.dot(schoolsize_values,schoolsize_distribution)
    schoolsize_values = np.floor((avgSchoolsize/mean_school_size)*schoolsize_values).astype(int)
    
    return schoolsize_values, schoolsize_distribution


# compute haversine distance
def distance(lat1, lon1, lat2, lon2):
    radius = 6371 # km

    dlat = math.radians(lat2-lat1)
    dlon = math.radians(lon2-lon1)
    a = math.sin(dlat/2) * math.sin(dlat/2) + math.cos(math.radians(lat1))         * math.cos(math.radians(lat2)) * math.sin(dlon/2) * math.sin(dlon/2)
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
    d = radius * c

    return d

def workplaces_size_distribution(a=3.26, c=0.97, m_max=2870):
    count=1
    a=3.26
    c=0.97
    m_max=2870
    p_nplus = np.arange(float(m_max))
    for m in range(m_max):
        p_nplus[m] =  ((( (1+m_max/a)/(1+m/a))**c) -1) / (((1+m_max/a)**c) -1)

    p_nminus = 1.0 - p_nplus
    p_n = np.arange(float(m_max))
    prev=0.0
    for m in range(1, m_max):
        p_n[m] = p_nminus[m] - prev
        prev = p_nminus[m]

    return p_n/sum(p_n)


def travel_distance_distribution(m_min,m_max,a,b):
    temp = []
    for d in np.arange(m_min,m_max,1):
        temp.append(1/(1+(d/a)**b))
    temp = np.array(temp)
    return temp/np.sum(temp)


# Create dataframes for validation
with open('data/bangalore/individuals.json', 'r') as f:
  data = json.load(f)



df1 = pd.DataFrame(data)

wp = pd.read_json('data/bangalore/workplaces.json')
sid = pd.read_json('data/bangalore/schools.json')['ID'].values[-1]


print('\nGenerating validation plots for the instantitaion...\n')


with open('data/base/bangalore/cityProfile.json', newline='') as file:
    cityprofiledata = json.load(file)
    
a_workplacesize = 3.26
c_workplacesize = 0.97
m_max_workplacesize = 2870
avgSchoolsize = 300
m_max_commuter_distance = cityprofiledata['maxWorkplaceDistance']
a_commuter_distance = 10.751
b_commuter_distance = 5.384

# Get distributions to match
age_values, age_distribution = compute_age_distribution(cityprofiledata['age']['weights'])
household_sizes, household_distribution = compute_household_size_distribution(cityprofiledata['householdSize']['bins'], cityprofiledata['householdSize']['weights'])
schoolsize_values, schoolsize_distribution = extrapolate_school_size_distribution(cityprofiledata['schoolsSize']['weights'],avgSchoolsize)
workplacesize_distribution = workplaces_size_distribution()

print("Validating age distribution in instantiation...",end='',flush=True)
plt.plot(df1['age'].value_counts(normalize=True).sort_index(ascending=True), 'r-o',label='Instantiation')
plt.plot(age_distribution, 'b-',label='Data')
plt.xlabel('Age')
plt.ylabel('Density')
plt.title('Distribution of age')
plt.grid(True)
plt.legend()
plt.xticks(np.arange(0,81,10), np.concatenate((age_values[np.arange(0,71,10)], ['80+'])) )
plt.savefig('age.png')
plt.close()
print("done.",flush=True)

print("Validating household-size in instantiation...",end='',flush=True)
house = df1['household'].value_counts().values
unique_elements, counts_elements = np.unique(house, return_counts=True)
counts_elements = counts_elements / np.sum(counts_elements)
plt.plot(counts_elements, 'r-o', label='Instantiation')
plt.plot(household_distribution, 'b-', label='data')
plt.xlabel('Household-size')
plt.ylabel('Density')
plt.title('Distribution of household-size')
plt.grid(True)
plt.legend()
plt.xticks(np.arange(0,len(household_sizes),1), np.concatenate((age_values[np.arange(1,household_sizes[-1],1)], [str(household_sizes[-1])+'+'])) )
plt.savefig('household_size.png')
plt.close()
print("done.",flush=True)

print("Validating school-size in instantiation...",end='',flush=True)
schoolsizeDistribution = cityprofiledata['schoolsSize']['weights']
full_frame = np.floor(np.array([len(np.where(df1['school'] == i)[0]) for i in np.unique(df1['school'].values)[~np.isnan(np.unique(df1['school'].values))]])/100).astype(int)
schoolsize_output = [len(np.where(full_frame == j)[0]) for j in np.arange(0,len(schoolsizeDistribution))] / np.sum([len(np.where(full_frame == j)[0]) for j in np.arange(0,len(schoolsizeDistribution))])
plt.plot(schoolsize_output,'r-o', label='Instantiation')
plt.plot(schoolsizeDistribution,'b-', label='Data')
xlabel = np.arange(0,len(schoolsizeDistribution))
plt.xticks(xlabel, np.concatenate((np.arange(1,10)*100, [str('901+')])))
plt.xlabel('School size')
plt.ylabel('Density')
plt.legend()
plt.title('Distribution of school size')
plt.grid(True)
plt.savefig('school_size.png')
plt.close()
print("done.",flush=True)


# generate workplace size distribution
a=a_workplacesize
c=c_workplacesize
m_max=m_max_workplacesize
workplace_sizes = np.arange(m_max)
p_nplus = np.arange(float(m_max))
for m in range(m_max):
    p_nplus[m] =  ((( (1+m_max/a)/(1+m/a))**c) -1) / (((1+m_max/a)**c) -1)

p_nminus = 1.0 - p_nplus
p_n = np.arange(float(m_max))
prev=0.0
for m in range(1, m_max):
    p_n[m] = p_nminus[m] - prev
    prev = p_nminus[m]


# workplace size
print("Validating workplace-size in instantiation...",end='',flush=True)

full_frame = np.array([len(np.where(df1['workplace'] == i)[0]) for i in np.unique(df1['workplace'].values)[~np.isnan(np.unique(df1['workplace'].values))]])
workplacesize_output = [len(np.where(full_frame == j)[0]) for j in workplace_sizes] / np.sum([len(np.where(full_frame == j)[0]) for j in workplace_sizes])
workplace_distribution = p_n
plt.plot(np.log10(workplace_sizes),np.log10(workplacesize_output),'r',label='Instantiation')
plt.plot(np.log10(workplace_sizes), np.log10(workplace_distribution),label='Model')
plt.xlabel('Workplace size (log-scale)')
plt.ylabel('log_10 Density')
plt.title('Distribution of workplace size (in log-scale)')
plt.grid(True)
plt.legend()
plot_xlabel =  [1, 10, 100, 1000, 2400]
plot_xlabel1 = np.log10(workplace_sizes)[plot_xlabel]
plt.xticks(plot_xlabel1, (workplace_sizes)[plot_xlabel])
plt.savefig('workplace_size.png')
plt.close()
print("done.",flush=True)

print("Validating workplace commute distance in instantiation...",end='',flush=True)
full_frame = np.array([distance(df1.loc[i,'lat'],df1.loc[i,'lon'],wp.loc[wp['id']==int(df1.loc[i,'workplace']),'lat'].values[0],wp.loc[wp['id']==int(df1.loc[i,'workplace']),'lon'].values[0]) for i in np.where(df1['workplaceType']==1)[0]])
commuter_distance_output = [len(np.where(np.array(np.floor(full_frame),dtype=int) ==i)[0]) for i in np.arange(0,m_max_commuter_distance)]/np.sum([len(np.where(np.array(np.floor(full_frame),dtype=int) ==i)[0]) for i in np.arange(0,m_max_commuter_distance)])
actual_dist=[]
actual_dist = travel_distance_distribution(0,m_max_commuter_distance,a_commuter_distance,b_commuter_distance)
d = np.arange(0,m_max_commuter_distance,1)
plt.plot(np.log10(d),np.log10(actual_dist),'b-',label='Model')
plt.plot(np.log10(d),np.log10((commuter_distance_output)),'r-o',label='Instantiation')
plt.xlabel('Workplace distance (km) (log-scale)')
plt.ylabel('log_10 Density')
plt.title('Distribution of workplace distances')
plot_xlabel=[1,5,25,31]
plot_xlabel1 = np.log10(d)[plot_xlabel]
plt.xticks(plot_xlabel1,d[plot_xlabel])
plt.grid(True)
plt.legend()
plt.savefig('workplace_distance.png')
plt.close()
print("done.",flush=True)
