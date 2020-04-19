## `staticInst/` - Generates static files to instantiate a city based on Demographics data
The instantiation of a city is done by using the demographic data, information on the distribution and the number of households and the information of workplace distribution, school distribution and the employment statistics for a city. We look for the demographic data which contains data on the total population, number of children, the number of people who are employed, on a fine-grained geographic scope of ward or block of the city. 

The script to instantiate a city is written in Python and the following are the steps to setup a running instance of the script.

#### Setting up the virtualenv
The first step will be to setup the virtualenv inside which we install the required python packages  specified in the `staticInst/requirements.txt` file.
Let us create a virtualenv named `edaDev`, you could replace it with a different name too,

`python3 -m venv edaDev` 

The above statement would create a new directory under `markov_simuls` with the path `markov_simuls/edaDev`. This directory will contain the python packages you install and also helps to create an isolated python environment without affecting your other packages. 
If you are comfortable with Anaconda, there are lot of guides to help you [setup a virtualenv](https://uoa-eresearch.github.io/eresearch-cookbook/recipe/2014/11/20/conda/). The next step will be to activate the virtual environment `edaDev` and install the required packages there, which is done with the following commands:

```
source edaDev/bin/activate
```
```
pip install -r staticInst/requirements.txt
```

#### Data Needs
The following table lists the data needs for the instantiation script to run 

| Dataset Description| Required Fields in the Data | File Format |
|--------------------|-----------------------------|-------------|
|Ward boundaries of a city | ward no, ward name, geometry(Multipolygon Geometry) | GeoJSON|
|Age Distribution (bins of 5), household size distribution for the  city| -  |JSON|
|Demographic data for all wards of the city|ward no, total population in the ward, area of the ward, total number of households per ward | CSV |
|Employment data for all wards of the city|ward no, total population employed, total population unemployed| CSV |
|Common Areas (or) Points of Interest in the City| latitude, longitude of common areas like transport communities, markets, restaurants, places of worship) per ward (if possible) | CSV|
|Commute distance between wards (optional)| Origin-Distance Matrix (pair-wise number of people moving between wards) for all wards in the city | CSV|
The above data for Bangalore and Mumabi have been processesd and stored in './data/base/bangalore' and './data/base/mumbai' respectively.

#### Instantiating a city
After setting up the environment, we are ready to run the script to instantiate a city. We will be creating a instantiate for Bangalore city for a population of 100,000 people with additional input parameters:

- input directory `-i`: `data/base/bangalore`
- output directory `-o`: `data/bangalore-100K`

To instantiate a Bangalore city with the mentioned configurations run the command

```python parse_and_instantiate.py -c city_name -n target_population -i inputPath -o outputPath```

The above script instantiates a synthetic Bangalore city where the population of 100,000 people are randomly distributed acroos the 198 wards of the city with each individual being assigned to a house, school, workplace and community centre based on their age, and commute distance. The instantiated outputs are in the form of JSON files and will be available in the specified output directory (or) `staticInst/data/bangalore-100K`. 


If the input parameters are not specified, the following default parameters will be used

```
city_name = 'bangalore'
target_population = 100000
inputPath = './data/base/bangalore/'
outputPath = './data/bangalore-100K/'
```

#### Sub-Directory Structure
The sub-directory structure followed for storing and processing of static data source used for instantiations is outlined as follows. 


```
|
|- staticInst/
   |- data/
      |- base/                          # Raw data for each city
         |- bangalore/                  # Data for Banglore City
            |- demographics.csv         # demographic data about each ward 
            |- households_and_age.json  # age and household-size distributions
            |- common_areas.csv         # location of places where people congrugate
            |- city.geojson             # geographic boundaries of wards
            |- employment.csv           # census data on employed people
            |- cityProfile.json         # processed demographic data dump for all wards
         |- mumbai/                     # Data for Banglore City
            |- demographics.csv         # demographic data about each ward 
            |- households_and_age.json  # age and household-size distributions
            |- common_areas.csv         # location of places where people congrugate
            |- city.geojson             # geographic boundaries of wards
            |- employment.csv           # census data on employed people
            |- cityProfile.json         # processed demographic data dump for all wards
      |      
      |- bangalore-100K/                # Data used for instantiating banglore city
         |- workplaces.json             # instantiation of workplaces
         |- commonArea.json             # instantiation of commonplaces
         |- households.json             # instantiation of households
         |- schools.json                # instantiation of schools
         |- individuals.json            # instantiation of individuals
         |- map.geojson                 # instantiation of individuals
   |- computeDistributions.py           # module to get distributions from the cityProfile.json
   |- parse_and_instantiate.py          # script to instantiate a city
```

**Suggestion**: For consistency, we can have naming convention in lowercase-only with underscore separators. 

