# Preparing the data to instantiate


#### Sub-Directory Structure
The sub-directory structure followed for storing and processing of static data source used for instantiations is outlined as follows.


```
|- staticInst/
   |- data/
      |- base/                            # Raw data for each city
         |- bangalore/                    # Data for Bangalore City
            |- demographics.csv           # demographic data about each ward
            |- households.csv             # number of households in each ward
            |- ODMatrix.csv               # origin-destination matrix for employed
            |- city.geojson               # geographic boundaries of wards
            |- employment.csv             # census data on employed people
            |- cityProfile.json           # age, household size and school size distributions
         |- mumbai/                       # Data for Mumbai City, with each of the 24 wards split into non-HD areas and HD-areas
            |- demographics.csv           # demographic data about each ward division (48 total)
            |- households.json            # number of households in each ward
            |- ODMatrix.csv               # origin-destination matrix for employed
            |- city.geojson               # geographic boundaries of wards (24 total)
            |- employment.csv             # census data on employed people
            |- cityProfile.json           # age, household size and school size distributions
            |- presampled-points/         # presampled latitude-longitudes for 48 ward subdivisions
               |- 0.csv                   # presampled points for ward index 0
               ...
               |- 47.csv                  # presampled points for ward index 47
               |- PresamplePoints.ipynb   # python notebook for presampling points from wards subdivisions
               |- slumClusters.geojson    # geographic boundaries of slum clusters
```

## Demographic Data

## Geographic

## Mobility Data

## Disease Progression Data