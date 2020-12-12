#!/usr/bin/env python
# coding: utf-8

import numpy as np
import pickle

import json
import pandas as pd
import geopandas as gpd
import argparse

import math
from shapely.geometry import Point, MultiPolygon

import os
import sys
from pathlib import Path

import matplotlib.pyplot as plt
from computeDistributions import *

from functools import wraps
from time import time

import warnings
warnings.simplefilter('error')

def measure(func):
    @wraps(func)
    def _time_it(*args, **kwargs):
        start = time()
        try:
            print(f"{func.__name__.ljust(30)}...\t", end='', flush=True)
            return func(*args, **kwargs)
        finally:
            end = int((time() - start)*1000)
            print(f"done. ({end} ms)")
    return _time_it


inputfiles = {
    "citygeojson":"city.geojson",
    "demographics":"demographics.csv",
    "employment":"employment.csv",
    "household":"households.csv",
    "cityprofile":"cityProfile.json",
    "ODMatrix":"ODMatrix.csv"
    }

outputfiles = {
    "individuals":"individuals.json",
    "houses":"houses.json",
    "workplaces":"workplaces.json",
    "schools":"schools.json",
    "wardCentreDistance":"wardCentreDistance.json",
    "commonArea":"commonArea.json",
    "fractionPopulation":"fractionPopulation.json",
    "PRG_np_random_state":"PRG_np_random_state.bin",
    }

workplacesTypes = {
    None: 0,
    "office": 1,
    "school": 2
    }

officeType = {
    "Other":0,
    "SEZ":1,
    "Government":2,
    "IT":3,
    "Construction":4,
    "Medical":5
    }



def fileExists(path):
    return os.path.isfile(path)

def folderExists(path):
    return os.path.exists(path)

def normalise(raw): 
    # Scale everything so that the array sums to 1
    # It doesn't quite, due to floating point errors, but 
    # np.random.choice does not complain anymore.
    s = sum([float(i) for i in raw]); return [float(i)/s for i in raw]

def sampleBinsWeights(bins,weights):
    assert len(bins) == len(weights)

    s = str(np.random.choice(bins,1,p=weights)[0])
    if '+' in s:
        return int(s[:-1])+1
        #This is the last bucket, something like x+. Choosing (x+1) by default.
    elif '-' in s:
        (a,b) = s.split('-')
        return np.random.randint(int(a),int(b)+1) #np.random.randint chooses from halfopen interval
    else:
        return int(s)

def distance(lat1, lon1, lat2, lon2):
    radius = 6371 # km

    dlat = math.radians(lat2-lat1)
    dlon = math.radians(lon2-lon1)
    a = (math.sin(dlat/2) * math.sin(dlat/2) + math.cos(math.radians(lat1)) 
         * math.cos(math.radians(lat2)) * math.sin(dlon/2) * math.sin(dlon/2))
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
    d = radius * c

    return d

def workplaces_size_distribution(a=3.26, c=0.97, m_max=2870):
    # This is a particular version of the power law:
    # Pr[ m > x ] is proportional to x^{-c}. 
    # This is additionally slightly adjusted in this implementation
    # so that the max workplace size = m_max, and the size is always 
    # at least 1. 
    # RP: There is also a scaling by a parameter 'a', which I don't 
    # follow why, but leaving the implementation as earlier.
    # RP: should add a reference for why these a,c,m_max were chosen.

    mirror_cdf = np.zeros(m_max, dtype=float)   # mirror_cdf[i] = Pr[ size > i]
    for m in range(m_max):
        mirror_cdf[m] =(  (((1 + (m_max/a))/((1 + (m/a)))**c) - 1) / 
                        (((1 + (m_max/a))**c)-1))

    p_n = np.insert((np.diff(mirror_cdf) * (-1)), 0, 0)  
    # np.diff computes a[i+1] - a[i]. We want a[i] - a[i+1]
    assert len(p_n)==m_max
    return p_n / sum(p_n)



class City:

    def reset(self):
        self.state_np_random = None
        #Default values:

        #ppl working at sez and gov (Bangalore data)
        self.max_sez= 230000 /10
        self.max_gov= (2295000*(12.327/66.84)*0.5) /10
        self.max_ites = 1500000 /10
        self.max_ites_not_sez = self.max_ites- self.max_sez
        # Can we add a reference for these numbers?


        # Feature request: Read these commuter distance params from cityprofile.json
        self.a_commuter_distance = 4 #parameter in distribution for commuter distance - Thailand paper
        self.b_commuter_distance = 3.8  #parameter in distribution for commuter distance - Thailand paper
        self.m_max_commuter_distance = None

        self.nwards = None
        self.totalPop = None

        self.wardData = None
        self.ODMatrix = None

        self.householdsize_bins = None
        self.householdsize_weights = None
        self.different_age_bins = False
        self.age_bins = None
        self.age_weights = None
        self.age_slum_bins = None
        self.age_slum_weights = None
        self.ageGivenHHDist = None
        self.schoolsize_bins = None
        self.schoolsize_weights = None

        self.has_slums = False

        self.presampled_points = None
        self.community_centres = None

        self.workers = None
        self.schoolers = None

        #This is what we will eventually generate
        self.houses = None
        self.num_houses = None
        self.individuals = None
        self.num_individuals = None
        self.workplaces = None
        self.num_workplaces = None
        self.schools = None
        self.num_schools = None
        self.num_workers = None

    
    def save_random_seeds(self):
        self.state_np_random = np.random.get_state()

    def set_random_seeds(self, folder):
        print(f"Restoring random seeds from {folder}.")
        with open(os.path.join(folder, outputfiles['PRG_np_random_state']), 'rb') as f:
            np.random.set_state(pickle.load(f))


    def checkRows(self, df, name="df"):
        if self.nwards is None:
            for i in range(df.shape[0]):
                assert df["wardNo"].iloc[i] == i+1,f"Mismatch in {name}: row {i} has wardNo {df['wardNo'].iloc[i]}"
        else:
            assert df.shape[0] == self.nwards, f"Mismatch in {name}: num_rows is not {nwards}"
            for i in range(self.nwards):
                assert df["wardNo"].iloc[i] == i+1,f"Mismatch in {name}: row {i} has wardNo {df['wardNo'].iloc[i]}"

    def check_merged_df(self, df, name):
        nrows = df.shape[0]
        for i in range(nrows):
            assert df['wardNo_x'].iloc[i] == df['wardNo_y'].iloc[i], f"Merge mismatch in row {i} of {name}"

    def reorder_wardData_Rows(self):
        self.wardData = (self.wardData
                         .sort_values('wardIndex')
                         .reset_index(drop=True))

    def set_demographics(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["demographics"])), f"{inputfiles['demographics']} missing"

        demographics = pd.read_csv(Path(input_dir,inputfiles["demographics"]))
        necessary_cols = ["wardNo", "wardName", "totalPopulation"]
        for col in necessary_cols:
            assert col in demographics.columns
        if "hd_flag" in demographics.columns:
            self.has_slums = True
            demographics["hd_flag"] = demographics["hd_flag"].astype(int)
            necessary_cols += ["hd_flag"]
        else:
            self.has_slums = False
        demographics = demographics[necessary_cols]
        demographics["wardNo"] = demographics["wardNo"].astype(int)
        demographics["wardIndex"] = demographics["wardNo"] - 1 # To work with a 0-indexed column
        demographics['totalPopulation'] = demographics['totalPopulation'].astype(int)

        self.nwards = demographics['wardIndex'].shape[0]
        self.totalPop = demographics['totalPopulation'].sum()
        self.wardData = demographics
                
    def set_employments(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["employment"])), f"{inputfiles['employment']} missing"
        
        employments = pd.read_csv(Path(input_dir,inputfiles["employment"]))
        necessary_cols = ["wardNo", "wardName", "Employed"]
        for col in necessary_cols:
            assert col in employments.columns
        employments = employments[necessary_cols]

        employments["wardNo"] = employments["wardNo"].astype(int)
        employments['Employed'] = employments['Employed'].astype(int)

        self.wardData = self.wardData.merge(
            employments, 
            on="wardName",
            validate="one_to_one")
        
        self.check_merged_df(self.wardData, "employment.csv")
        self.wardData = (self.wardData
                             .drop(['wardNo_y'], axis=1)
                             .rename(columns={'wardNo_x':'wardNo'})
                            )
    
    def set_geoDF(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["citygeojson"])), f"{inputfiles['citygeojson']} missing"
        
        geoDF = gpd.read_file(Path(input_dir,inputfiles["citygeojson"]))

        necessary_cols = ['wardNo', 'wardName', 'geometry']
        for col in necessary_cols:
            assert col in geoDF.columns
        geoDF = geoDF[necessary_cols]
        geoDF['wardBounds'] = geoDF.apply(lambda row: MultiPolygon(row['geometry']).bounds, axis=1)
        geoDF['wardCentre'] = geoDF.apply(
            lambda row: (
                MultiPolygon(row['geometry']).centroid.x, 
                MultiPolygon(row['geometry']).centroid.y
                ), 
            axis=1
            )
        geoDF['wardNo'] = geoDF['wardNo'].astype(int)

        self.wardData = self.wardData.merge(
            geoDF, 
            on="wardName",
            validate="one_to_one")
        
        self.check_merged_df(self.wardData, "city.geojson")
        self.wardData = (self.wardData
                             .drop(['wardNo_y'], axis=1)
                             .rename(columns={'wardNo_x':'wardNo'})
                            )

    def set_ODMatrix(self, input_dir):
        assert self.nwards is not None
        
        if fileExists(Path(input_dir, inputfiles["ODMatrix"])):
            #do something
            ODMatrix = pd.read_csv(Path(input_dir,inputfiles['ODMatrix'])).sort_values('wardNo')
            self.checkRows(ODMatrix,"ODMatrix")
            
            cols = [a for a in ODMatrix.columns if a != "wardNo"]
            for i in range(self.nwards):
                assert int(cols[i]) == i+1, f"Mismatch in ODMatrix.csv: col {i+1} has {cols[i]}"
            
            _ = ODMatrix.pop("wardNo").astype(float)

            self.ODMatrix = ODMatrix.values
        else:
            self.ODMatrix = [[(1/self.nwards) for i in range(self.nwards)] for j in range(self.nwards)]
            
        for i in range(self.nwards):
            self.ODMatrix[i] = normalise(self.ODMatrix[i])
       
    def set_presampled_points(self, input_dir):
        assert folderExists(Path(input_dir,'presampled-points')), "'presampled-points' missing"
        assert self.nwards is not None
        
        self.presampled_points = []
        for i in range(self.nwards):
            assert fileExists(Path(input_dir,"presampled-points",f"{i}.csv")), f"presampled-points/{i}.csv missing"
            df = pd.read_csv(Path(input_dir,"presampled-points",f"{i}.csv"),names=["lat","lon"]).astype(float)
            self.presampled_points.append(df)
    
    def processAgeGivenHH(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["cityprofile"])), f"{inputfiles['cityprofile']} missing"
        with open(Path(input_dir, inputfiles["cityprofile"]),"r") as file:
            cityprofiledata = json.load(file)
        assert "ageGivenHouseholdSize" in cityprofiledata.keys()
        
        ageGivenHHDist = cityprofiledata['ageGivenHouseholdSize']['weights']
        for i in range(len(ageGivenHHDist)):
            ageGivenHHDist[i] = normalise(ageGivenHHDist[i])
            assert len(ageGivenHHDist[i]) == len(self.age_bins)
        print("(age_given_household_size_distribution provided)")
        self.ageGivenHHDist = ageGivenHHDist
        
    def set_city_profile(self, input_dir):
        assert fileExists(Path(input_dir, inputfiles["cityprofile"])), f"{inputfiles['cityprofile']} missing"
        with open(Path(input_dir, inputfiles["cityprofile"]),"r") as file:
            cityprofiledata = json.load(file)
        
        if "city" in cityprofiledata.keys():
            self.name = cityprofiledata['city']
        else:
            self.name = "unknown"
            
        if ("distance_kernel_a" in cityprofiledata.keys() 
            and "distance_kernel_b" in cityprofiledata.keys()):
            print("(Distance kernel parameters provided.)")
            self.a_commuter_distance = cityprofiledata['distance_kernel_a']
            self.b_commuter_distance = cityprofiledata['distance_kernel_b']
        else:
            self.a_commuter_distance = 4   # From the Thailand paper
            self.b_commuter_distance = 3.8 # From the Thailand paper
            
        self.householdsize_bins = cityprofiledata['householdSize']['bins']
        self.householdsize_weights = normalise(cityprofiledata['householdSize']['weights'])
        assert len(self.householdsize_bins) == len(self.householdsize_weights), "household bins and weights differ in lengths"
        
        if "age_non_slum" in cityprofiledata:
            print("Using different age distributions for slums and non-slums.")
            self.different_age_bins = True
            self.age_bins = cityprofiledata['age_non_slum']['bins']
            self.age_weights = normalise(cityprofiledata['age_non_slum']['weights'])
            assert len(self.age_bins) == len(self.age_weights), "age_non_slum bins and weights differ in lengths"
        
            self.age_slum_bins = cityprofiledata['age_slum']['bins']
            self.age_slum_weights = normalise(cityprofiledata['age_slum']['weights'])
            assert len(self.age_slum_bins) == len(self.age_slum_weights), "age_slum bins and weights differ in lengths"
        else:
            self.age_bins = cityprofiledata['age']['bins']
            self.age_weights = normalise(cityprofiledata['age']['weights'])
            assert len(self.age_bins) == len(self.age_weights), "age bins and weights differ in lengths"    
            self.age_slum_bins = self.age_bins
            self.age_slum_weights = self.age_weights
        
        if "ageGivenHouseholdSize" in cityprofiledata.keys():
            self.processAgeGivenHH(input_dir)
    
        self.schoolsize_bins = cityprofiledata['schoolsSize']['bins']
        self.schoolsize_weights = normalise(cityprofiledata['schoolsSize']['weights'])
        assert len(self.schoolsize_bins) == len(self.schoolsize_weights), "schoolsSize bins and weights differ in lengths"        
        
        self.m_max_commuter_distance = cityprofiledata['maxWorkplaceDistance']
        
    def sampleRandomLatLon(self, wardIndex):
        if self.presampled_points is not None:
            i = np.random.randint(0,self.presampled_points[wardIndex].shape[0])
            (lat,lon) = self.presampled_points[wardIndex].iloc[i]
            return (lat,lon)
        else:
            (lon1,lat1,lon2,lat2) = self.wardData['wardBounds'].iloc[wardIndex]
            while True:
                lat = np.random.uniform(lat1,lat2)
                lon = np.random.uniform(lon1,lon2)
                point = Point(lon,lat) #IMPORTANT: Point takes in order of longitude, latitude
                if MultiPolygon(self.wardData['geometry'].iloc[wardIndex]).contains(point):
                    return (lat,lon)

    def rescale(self, n):
        assert self.wardData is not None 
        
        scale = n / self.totalPop
        
        self.wardData["totalPopulation"] = (self.wardData["totalPopulation"] * scale).astype(int)
        self.wardData["Employed"] = (self.wardData["Employed"] * scale).astype(int)

        self.totalPop = self.wardData['totalPopulation'].sum()
        
    def sampleAge_non_slum(self):
        assert self.age_bins is not None and self.age_weights is not None
        return sampleBinsWeights(self.age_bins, self.age_weights)
        
    def sampleAge_slum(self):
        assert self.age_slum_bins is not None and self.age_slum_weights is not None
        return sampleBinsWeights(self.age_slum_bins, self.age_slum_weights)

    def sampleAgeGivenHousehold(self, size):
        # Assuming that the bins for this are of the form 
        # [1,2,3,...,m-1,m+]. If not, this has to be modified.

        assert self.ageGivenHHDist is not None
        size_bucket = min(size, len(self.ageGivenHHDist)) - 1
        assert size_bucket >= 0
        
        return sampleBinsWeights(self.age_bins, self.ageGivenHHDist[size_bucket])
    
    def sampleHouseholdSize(self):
        assert self.householdsize_bins is not None and self.householdsize_weights is not None
        return sampleBinsWeights(self.householdsize_bins, self.householdsize_weights)

    def sampleSchoolSize(self):
        assert self.schoolsize_bins is not None and self.schoolsize_weights is not None
        return sampleBinsWeights(self.schoolsize_bins, self.schoolsize_weights)

    def sampleWorkplaceSize(self):
        wsdist = workplaces_size_distribution()
        m_max = len(wsdist)
        return int(np.random.choice(np.arange(m_max),1,p=wsdist)[0])

    def set_community_centres(self):
        assert self.nwards is not None
        community_centres = []
        
        for wardIndex in range(self.nwards):
            if self.presampled_points is not None:
                (lat,lon) = self.sampleRandomLatLon(wardIndex)
            else:
                (lon,lat) = self.wardData['wardCentre'].iloc[wardIndex]
                #IMPORTANT: shapely works with (lon, lat) order
            community_centres.append((lat,lon))
        self.community_centres = community_centres

    def getCommunityCenterDistance(self, lat, lon, wardIndex):
        assert self.community_centres is not None
        (latc,lonc) = self.community_centres[wardIndex]
        return distance(lat,lon,latc,lonc)
        
    @measure
    def createHouses(self):
        self.houses = []
        hid = 0
        for wardIndex in range(self.nwards):
            pop = self.wardData["totalPopulation"][wardIndex]
            currpop = 0

            #creating houses
            while(currpop < pop):
                h = {}
                h["id"]=hid
                h["wardIndex"]=wardIndex

                if self.has_slums:
                    h["slum"] = int(self.wardData["hd_flag"][wardIndex])

                s = self.sampleHouseholdSize()
                h["size"]=s
                currpop+=s

                (lat,lon) = self.sampleRandomLatLon(wardIndex)
                h["lat"] = lat
                h["lon"] = lon

                self.houses.append(h)
                hid+=1
        self.num_houses = hid
       
    @measure
    def populateHouses(self):
        assert self.houses is not None
        
        pid = 0
        self.individuals = []
        self.workers = [[] for _ in range(self.nwards)]
        self.schoolers = [[] for _ in range(self.nwards)]
        
        employed_frac = self.wardData["Employed"] / self.wardData["totalPopulation"]
        self.wardData["generatedPopulation"] = 0
        self.wardData["generatedEmployed"] = 0
        generatedPop = 0
        num_workers = 0
        
        for h in self.houses:
            size = h["size"]
            wardIndex = h["wardIndex"]
            for i in range(size):
                p = {
                    "id":pid,
                    "household":h["id"],
                    "wardIndex":wardIndex,
                    "wardNo":wardIndex + 1,
                    "lat": h["lat"],
                    "lon": h["lon"],
                    "CommunityCentreDistance": self.getCommunityCenterDistance(h["lat"],h["lon"],wardIndex),
                    #Setting some default values
                    "employed": 0,
                    "workplaceType": workplacesTypes[None]
                    }

                if self.has_slums:
                    p["slum"] = h["slum"]
                
                if self.ageGivenHHDist is not None:
                    age = self.sampleAgeGivenHousehold(size)
                elif (self.has_slums and p["slum"]==1):
                    age = self.sampleAge_slum()
                else:
                    age = self.sampleAge_non_slum()
                
                    
                # Currently, ages of household members chosen independently.
                
                p["age"] = age

                if age < 3:                         # toddlers stay at home
                    
                    p["employed"]=0
                    p["workplaceType"] = workplacesTypes[None]

                elif age >= 3 and age < 15 :        # decide about their school
                    
                    p["employed"]=0
                    p["workplaceType"] = workplacesTypes["school"]
                    #assuming they all go to school
                    self.schoolers[wardIndex].append(pid)

                elif age >= 15 and age < 65:        # decide about employment/school
                    
                    eprob = employed_frac.iloc[wardIndex]
                    if (self.has_slums and p["slum"]==1):
                        eprob_adjusted = eprob / sum([self.age_slum_weights[a] for a in range(self.age_slum_bins.index("15-19"),self.age_slum_bins.index("65-69"))])  #Probability that you are employed given 15 <= age < 65 
                    else:
                        eprob_adjusted = eprob / sum([self.age_weights[a] for a in range(self.age_bins.index("15-19"),self.age_bins.index("65-69"))])
                        
                        
                    if(np.random.uniform(0,1) < eprob_adjusted):
                        
                        #person is employed
                        p["employed"] = 1

                        # p["workplace"] = "TODO"
                        workplaceward = int(np.random.choice(
                            list(range(self.nwards)),1,p=self.ODMatrix[wardIndex])[0]
                                           )
                        p["workplaceward"] = workplaceward
                        p["workplaceType"] = workplacesTypes["office"]
                        self.workers[workplaceward].append(pid)
                        num_workers+=1
                        self.wardData.at[wardIndex,"generatedEmployed"]+=1
                    else:
                        p["employed"] = 0
                        if age < 20:
                            p["workplaceType"] = workplacesTypes["school"]
                            # All the unemployed in this age bracket go to school
                            self.schoolers[wardIndex].append(pid)
                        else:
                            p["workplaceType"] = workplacesTypes[None]
                else:
                    #decide about seniors
                    p["employed"] = 0
                    p["workplaceType"] = workplacesTypes[None]
                    
                self.individuals.append(p)
                self.wardData.at[wardIndex,"generatedPopulation"]+=1
                generatedPop +=1
                pid+=1
        self.num_individuals = generatedPop
        self.num_workers = num_workers
        
    def sampleOfficeType(self, size):
        num_gov = 0
        num_ites = 0 
        num_sez = 0
        # Previous version had these as global variables. Not sure what they were used for
        #Large workplace at SEZs, officeType=1
        if num_sez < self.max_sez and size >= 200 and size < 300 and np.random.binomial(1,0.7):
            num_sez += size
            return  officeType['SEZ']
        elif num_sez < self.max_sez and size >= 300 and size < 400 and np.random.binomial(1,0.8):
            num_sez += size
            return  officeType['SEZ']
        elif num_sez < self.max_sez and size >= 400 and size < 500 and np.random.binomial(1,0.9):
            num_sez += size
            return  officeType['SEZ']
        elif num_sez < self.max_sez and size >= 500:
            num_sez += size
            return  officeType['SEZ']

        #Government offices
        elif size >= 20 and num_gov < self.max_gov:
            num_gov += size
            return  officeType['Government']

        #IT/ITES
        elif size >= 10 and num_ites <= self.max_ites_not_sez:
            num_ites += size
            return  officeType['IT']
        else:
            return  officeType['Other']

    @measure
    def assignSchools(self):
        assert self.houses is not None
        assert self.individuals is not None
        
        self.schools = []
        sid = 0
        for wardIndex in range(self.nwards):
            while len(self.schoolers[wardIndex]) > 0: #some unassigned kids left in the ward
                #Set up basic facts about school to be created
                s = {"ID":sid} #capitalised in the previous code so keeping it so
                s["wardIndex"]=wardIndex
                (lat,lon) = self.sampleRandomLatLon(wardIndex)
                s["lat"] = lat
                s["lon"] = lon
                
                if self.has_slums:
                    s["slum"] = int(self.wardData["hd_flag"].iloc[wardIndex])

                size = self.sampleSchoolSize()

                #Fill up school with kids
                i = 0
                while(i < size and len(self.schoolers[wardIndex])>0):
                    pid = self.schoolers[wardIndex].pop(
                            np.random.randint(len(self.schoolers[wardIndex]))
                            )
                    self.individuals[pid]["school"] = sid
                    i+=1
                self.schools.append(s)
                sid+=1
                #Note: This sort of creates a very skewed first-bracket for school size.
                #If the city size is small, then many schools will be "under-capacity".
                #Need to think about how to fix this corner case.

        self.num_schools = sid

    @measure
    def assignWorkplaces(self):
        assert self.houses is not None
        assert self.individuals is not None
        assert self.schools is not None
        
        self.workplaces = []
        count = 0
        for wardIndex in range(self.nwards):
            num_ward_workers = len(self.workers[wardIndex])
            while len(self.workers[wardIndex])>0:
                wid = count + self.num_schools
                w = {
                    "id":wid,
                    "wardIndex":wardIndex
                }

                (lat,lon) = self.sampleRandomLatLon(wardIndex)
                w["lat"] = lat
                w["lon"] = lon
                
                s = self.sampleWorkplaceSize()
                oType = self.sampleOfficeType(s)
                w["officeType"]=oType

                i = 0
                while(i < s and len(self.workers[wardIndex])>0):
                    pid = self.workers[wardIndex].pop(np.random.randint(len(self.workers[wardIndex])))
                    self.individuals[pid]["workplace"] = wid
                    del self.individuals[pid]["workplaceward"]
                    i+=1
                self.workplaces.append(w)
                count+=1
        self.num_workplaces = count
    
    def describe(self):
        print(f"City: {self.name}")
        print(f"Population: {self.num_individuals}")
        print(f"Number of wards: {self.nwards}")
        print(f"Has slums: {self.has_slums}")
        print("")
        print(f"Number of houses: {self.num_houses}")
        print(f"Number of schools: {self.num_schools}")
        print(f"Number of workplaces: {self.num_workplaces}")
        print(f"Number of workers: {self.num_workers}")
        print("")

    def generate(self, n):
        assert self.wardData is not None
        assert self.ODMatrix is not None
        
        self.rescale(n)
        self.createHouses()
        self.populateHouses()
        self.assignSchools()
        self.assignWorkplaces()
        print("")
        self.describe()
        
    @measure
    def dump_files(self, output_dir):
        assert self.houses is not None
        assert self.individuals is not None
        assert self.schools is not None
        assert self.workplaces is not None
        
        assert output_dir is not None
               
        
        Path(output_dir).mkdir(parents = True, exist_ok = True)
        
        commonAreas = []
        for i in range(self.nwards):
            c = {"ID":i}
            c["wardNo"] = i+1
            (lon,lat)= self.community_centres[i]
            c["lat"] = lat
            c["lon"] = lon
            commonAreas.append(c)
        
        fractionPopulations = []
        for i in range(self.nwards):
            w = {"wardNo":i+1}
            w["totalPopulation"] = int(self.wardData["generatedPopulation"].iloc[i])
            w["fracPopulation"] = float(self.wardData["generatedPopulation"].iloc[i] / self.num_individuals)
            fractionPopulations.append(w)
        
        wardCentreDistances = [ {"ID":i+1} for i in range(self.nwards)]
        for i in range(self.nwards):
            for j in range(self.nwards):
                wardCentreDistances[i][str(j+1)] = distance(commonAreas[i]["lat"],
                                                            commonAreas[i]["lon"],
                                                            commonAreas[j]["lat"],
                                                            commonAreas[j]["lon"])

        with open(os.path.join(output_dir,outputfiles['houses']), "w+") as f:
            f.write(json.dumps(self.houses))
        with open(os.path.join(output_dir,outputfiles['individuals']), "w+") as f:
            f.write(json.dumps(self.individuals))
        with open(os.path.join(output_dir,outputfiles['schools']), "w+") as f:
            f.write(json.dumps(self.schools))
        with open(os.path.join(output_dir,outputfiles['workplaces']), "w+") as f:
            f.write(json.dumps(self.workplaces))
        with open(os.path.join(output_dir,outputfiles['commonArea']), "w+") as f:
            f.write(json.dumps(commonAreas))
        with open(os.path.join(output_dir,outputfiles['fractionPopulation']), "w+") as f:
            f.write(json.dumps(fractionPopulations))
        with open(os.path.join(output_dir,outputfiles['wardCentreDistance']), "w+") as f:
            f.write(json.dumps(wardCentreDistances))     
        with open(os.path.join(output_dir,outputfiles['PRG_np_random_state']), "wb+") as f:
            pickle.dump(self.state_np_random,f)

        
    def __init__(self, input_dir, random_seed_dir = None):
        self.reset()
        
        self.set_city_profile(input_dir)

        if random_seed_dir is not None:
            assert fileExists(
                os.path.join(random_seed_dir, outputfiles['PRG_np_random_state'])
                ), f"{os.path.join(random_seed_dir, outputfiles['PRG_np_random_state'])} not found"
            self.set_random_seeds(random_seed_dir)

        self.save_random_seeds()
        self.set_demographics(input_dir)
        self.set_employments(input_dir)
        self.set_ODMatrix(input_dir)
        if folderExists(Path(input_dir,'presampled-points')):
            self.set_presampled_points(input_dir)
        else:
            self.set_geoDF(input_dir) 
        self.reorder_wardData_Rows()
        self.set_community_centres()


# In[ ]:


@measure
def validate_slum_ages(city, df_ind, plots_folder=None):
    age_values, age_distribution = compute_age_distribution(city.age_slum_weights)
    df_slum = df_ind[df_ind['slum']==1]
    plt.plot(
        (df_slum['age']
         .value_counts(normalize=True)
         .sort_index(ascending=True)),
        'r-o',
        label='Instantiation')
        
    plt.plot(age_values, age_distribution, 'b-',label='Data')
    plt.xlabel('Slum_Age')
    plt.ylabel('Density')
    plt.title('Distribution of age')
    plt.grid(True)
    plt.legend()
    plt.xticks(np.arange(0,81,10), np.concatenate((age_values[np.arange(0,71,10)], ['80+'])) )
    if plots_folder is not None: 
        plt.savefig(os.path.join(plots_folder, 'age_slum.png'))
    else:
        plt.show()
    plt.close()
    
@measure
def validate_non_slum_ages(city, df_ind, plots_folder=None):
    age_values, age_distribution = compute_age_distribution(city.age_weights)
    if city.has_slums:
        df_non_slum = df_ind[df_ind['slum']==0]
    else:
        df_non_slum = df_ind
    plt.plot(
        (df_non_slum['age']
         .value_counts(normalize=True)
         .sort_index(ascending=True)),
        'r-o',
        label='Instantiation')
    
    plt.plot(age_values, age_distribution, 'b-',label='Data')
    plt.xlabel('Age')
    plt.ylabel('Density')
    plt.title('Distribution of age')
    plt.grid(True)
    plt.legend()
    plt.xticks(np.arange(0,81,10), np.concatenate((age_values[np.arange(0,71,10)], ['80+'])) )
    if plots_folder is not None: 
        plt.savefig(os.path.join(plots_folder, 'age_non_slum.png'))
    else:
        plt.show()
    plt.close()


@measure
def validate_householdsizes(city, df_ind, plots_folder=None):    
    household_sizes, household_distribution = compute_household_size_distribution(
        city.householdsize_bins, 
        city.householdsize_weights
    )
    plt.plot((df_ind
              .groupby('household')['id']
              .count()
              .value_counts(normalize=True)
              .sort_index(ascending=True)),
        color="red",
        marker="o",
        label="Instantiation"        
    )
    plt.plot(household_sizes , household_distribution, color="blue", label='Data')

    plt.xlabel('Household-size')
    plt.ylabel('Density')
    plt.title('Distribution of household-size')
    plt.grid(True)
    plt.legend()

    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'household_size.png'))
    else:
        plt.show()
    plt.close()

@measure
def validate_schoolsizes(city, df_ind, plots_folder=None):
    weights = city.schoolsize_weights
    df = ((df_ind
          .dropna(subset=['school'])
          .groupby('school')['id']
          .count()/100)
            .astype(int)
            .value_counts(normalize=True)
            .sort_index(ascending=True))
    plt.bar(df.index, height=df,
            width=0.5,
            color="red",
            alpha=0.5,
            label="Instantiation"
    )
    plt.plot(weights, label='Data', color="blue")
    
    bins = list(range(len(weights)))
    labels = [str(a*100) for a in bins[:-1]] + [str(bins[-1]*100)+'+']
    plt.xticks(bins[1:], labels[1:])
    plt.title("Distribution of school sizes")
    plt.grid(True)
    plt.legend()
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder, 'school_size.png'))
    else:
        plt.show()
    plt.close()

@measure
def validate_workplacesizes(city, df_ind, plots_folder=None):
    p_n = workplaces_size_distribution()
    plt.loglog(
        (df_ind
         .dropna(subset=['workplace'])
         .groupby('workplace')
         .count()['id']
         .value_counts(normalize=True)
         .sort_index(ascending=True)),
        color="red",
        label="Instantiation"        
    )
    plt.loglog(np.arange(len(p_n)), p_n, color="blue", label='Data (Zipf)')
    plt.title("Distribution of workplaces sizes")
    plt.xlim(right=len(p_n))
    plt.legend()
    plt.grid(True)
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'workplace_size.png'))
    else:
        plt.show()
    plt.close()

@measure
def validate_commutedistances(city, df_ind, df_work, plots_folder=None):
    df_merged = (df_ind
                 .dropna(subset=['workplace'])
                 .merge(
                     df_work,
                     left_on='workplace',
                     right_on='id'
                 )[['lat_x','lon_x','lat_y','lon_y']])
    df_merged['commute_distance'] = (df_merged.apply(
        lambda row: int(distance(
            row['lat_x'],
            row['lon_x'],
            row['lat_y'],
            row['lon_y'])),
        axis=1))
    plt.loglog(
        (df_merged['commute_distance']
         .value_counts(normalize=True)
         .sort_index(ascending=True)),
        color="red", marker="o",label='Instantiation')
    plt.ylabel("Density")
    plt.xlabel("Commute distance (in kms)")
    plt.title('Workplace commute distances')
    actual_dist = travel_distance_distribution(
        0,
        city.m_max_commuter_distance,
        city.a_commuter_distance,
        city.b_commuter_distance
        )
    plt.loglog(actual_dist, label='Data (distance kernel)')
    plt.legend()
    plt.grid(True)
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'workplace_distance.png'))
    else:
        plt.show()
    plt.close()

def validate(city, plots_folder=None):
    df_ind = pd.DataFrame(city.individuals)
    df_work = pd.DataFrame(city.workplaces)
    validate_non_slum_ages(city, df_ind,  plots_folder=plots_folder)
    if city.different_age_bins:
        validate_slum_ages(city, df_ind,  plots_folder=plots_folder)
    validate_householdsizes(city, df_ind, plots_folder=plots_folder)
    validate_schoolsizes(city, df_ind,  plots_folder=plots_folder)
    validate_workplacesizes(city, df_ind, plots_folder=plots_folder)
    validate_commutedistances(city, df_ind, df_work, plots_folder=plots_folder)


# In[ ]:


def main():
    
    default_pop = 100000
    default_ibasepath = 'data/base/bangalore/'
    default_obasepath = 'data/bangalore-100K/'
    my_parser = argparse.ArgumentParser(description='Create mini-city for COVID-19 simulation')
    my_parser.add_argument('-n', help='target population', default=default_pop)
    my_parser.add_argument('-i', help='input folder', default=default_ibasepath)
    my_parser.add_argument('-o', help='output folder', default=default_obasepath)
    my_parser.add_argument('--validate', help='script for validation plots on', action="store_true")
    my_parser.add_argument('--cohorts', help='[for cohorts] to instantiate cohorts in mumbai locals', action="store_true")
    my_parser.add_argument('-s', help='[for debug] restore random seed from folder', default=None)

    args = my_parser.parse_args()
    population = int(args.n)
    input_dir = args.i
    output_dir = args.o

    if len(sys.argv)==1:
        print("No arguments passed.\n")
        my_parser.print_help()
        print("\n Assuming default values.\n")

    print(f"input_folder: {input_dir}")
    print(f"output_folder: {output_dir}")
    print("")
    city = City(input_dir, random_seed_dir = args.s)
    city.generate(population)

    city.dump_files(output_dir)
    if args.validate:
        validate(city,output_dir)

if __name__ == "__main__":
    main()


# In[ ]:


###### OLDER VALIDATION Scripts. Keeping it for just comparison in case I missed something
@measure
def validate_old(city, plots_folder=None):
    ### I am just copying the validation scripts for now. 
    ### Not going through them carefully
    
    a_workplacesize = 3.26
    c_workplacesize = 0.97
    m_max_workplacesize = 2870
    avgSchoolsize = 300

    age_values, age_distribution = compute_age_distribution(city.age_weights)
    household_sizes, household_distribution = compute_household_size_distribution(
        city.householdsize_bins, 
        city.householdsize_weights
        )
    schoolsize_values, schoolsize_distribution = extrapolate_school_size_distribution(
        city.schoolsize_weights,
        avgSchoolsize
        )
    workplacesize_distribution = workplaces_size_distribution()
    
    df1 = pd.DataFrame(city.individuals)
    
    print("Validating age distribution in instantiation...",end='',flush=True)
    plt.plot(df1['age'].value_counts(normalize=True).sort_index(ascending=True), 'r-o',label='Instantiation')
    plt.plot(age_distribution, 'b-',label='Data')
    plt.xlabel('Age')
    plt.ylabel('Density')
    plt.title('Distribution of age')
    plt.grid(True)
    plt.legend()
    plt.xticks(np.arange(0,81,10), np.concatenate((age_values[np.arange(0,71,10)], ['80+'])) )
    if plots_folder is not None: 
        plt.savefig(os.path.join(plots_folder, 'age.png'))
    else:
        plt.show()
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
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'household_size.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)

    print("Validating school-size in instantiation...",end='',flush=True)
    schoolsizeDistribution = city.schoolsize_weights
    full_frame = np.floor(
        np.array([
                len(np.where(df1['school'] == i)[0]) for i in np.unique(
                    df1['school'].values
                    )[~np.isnan(np.unique(df1['school'].values))]
                ])/100
        ).astype(int)

    schoolsize_output = [
        len(np.where(full_frame == j)[0]) for j in np.arange(
            0,
            len(schoolsizeDistribution)
            )
        ] / np.sum([
                len(np.where(full_frame == j)[0]) for j in np.arange(
                    0,
                    len(schoolsizeDistribution)
                    )
                ])
    
    plt.plot(schoolsize_output,'r-o', label='Instantiation')
    plt.plot(schoolsizeDistribution,'b-', label='Data')
    xlabel = np.arange(0,len(schoolsizeDistribution))
    plt.xticks(xlabel, np.concatenate((np.arange(1,10)*100, [str('901+')])))
    plt.xlabel('School size')
    plt.ylabel('Density')
    plt.legend()
    plt.title('Distribution of school size')
    plt.grid(True)
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder, 'school_size.png'))
    else:
        plt.show()
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

    full_frame = np.array([
        len(np.where(df1['workplace'] == i)[0]) for i in np.unique(
            df1['workplace'].values
            )[~np.isnan(np.unique(df1['workplace'].values))]
        ])
    workplacesize_output = [len(np.where(full_frame == j)[0]) for j in workplace_sizes] / np.sum([
        len(np.where(full_frame == j)[0]) for j in workplace_sizes
        ])
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
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'workplace_size.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)
    
    
    wp = pd.DataFrame(city.workplaces)

    print("Validating workplace commute distance in instantiation...",end='',flush=True)
    full_frame = np.array([
        distance(
            df1.loc[i,'lat'],
            df1.loc[i,'lon'],
            wp.loc[(wp.index+city.num_schools)==int(df1.loc[i,'workplace']),'lat'].values[0],
            wp.loc[(wp.index+city.num_schools)==int(df1.loc[i,'workplace']),'lon'].values[0]
            ) for i in np.where(df1['workplaceType']==1)[0]
        ])
    commuter_distance_output = [
        len(np.where(np.array(np.floor(full_frame),dtype=int) ==i)[0]) for i in np.arange(
            0,
            city.m_max_commuter_distance
            )
        ] / np.sum([
                len(np.where(np.array(np.floor(full_frame),dtype=int) ==i)[0]) for i in np.arange(
                    0,
                    city.m_max_commuter_distance
                    )
                ])
    actual_dist=[]
    actual_dist = travel_distance_distribution(
        0,
        city.m_max_commuter_distance,
        city.a_commuter_distance,
        city.b_commuter_distance
        )
    d = np.arange(0,city.m_max_commuter_distance,1)
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
    if plots_folder is not None:
        plt.savefig(os.path.join(plots_folder,'workplace_distance.png'))
    else:
        plt.show()
    plt.close()
    print("done.",flush=True)

