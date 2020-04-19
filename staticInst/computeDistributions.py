#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#"""
#Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
#SPDX-License-Identifier: Apache-2.0
#"""
__name__ = "Module containing helper functions to validate city file instantiation"

import numpy as np 
import pandas as pd 


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

def travel_distance_distribution(m_min,m_max,a,b):
    temp = []
    for d in np.arange(m_min,m_max,1):
        temp.append(1/(1+(d/a)**b))
    temp = np.array(temp)
    return temp/np.sum(temp)


