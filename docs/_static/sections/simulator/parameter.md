# Simulator parameters

### Basic Parameters

The following are options that you should explicitly set.

- `--NUM_DAYS [num]`: The number of days for the simulator to run
- `--input_directory [directory]`: The directory (relative to where you are running the drive_simulator from) that contains the generated city files
- `--output_directory [directory]`: The directory (relative to where you are running the drive_simulator from) that contains the generated city files. The program will abort if this directory is not found. Please create the directory if it doesn’t exist.

### Seeding the initial infection

You can either seed the initial infection as a fraction of the total population, or you can seed a fixed number. You also have additional options to either choose to seed agents from HD-areas, or agents who take public transportation etc. via these options.

- `--SEED_FIXED_NUMBER`: A boolean flag to tell the simulator to seed a fixed number as opposed to seeding a fraction.
- `--INIT_FIXED_NUMBER_INFECTED [num]`: The number of infections to start the simulator with (requires the above --SEED_FIXED_NUMBER to be provided)
- `--INIT_FRAC_INFECTED [fraction]`: Seeding infections as a fraction of the total population

`--SEED_ONLY_NON_COMMUTER`: A boolean flag to tell the simulator to only seed infections among agents who do not take public transport
- `--SEED_HD_AREA_POPULATION`: A boolean flag to tell the simulator to include HD area residents in the pool of initial infections.

### Disease progression parameters

The simulator also has a bunch of options to change the default disease progression parameters. The only parameter here that we have changed is the fraction of exposed people who move to the symptomatic branch:

- `--SYMPTOMATIC_FRACTION [fraction]`: What fraction of the people who are exposed move to the symptomatic branch (that is, they do not recover within 5 days of being exposed).
The other options are the following (quite self-explanatory):

- `--MEAN_INCUBATION_PERIOD [arg]`
- `--MEAN_ASYMPTOMATIC_PERIOD [arg]`
- `--MEAN_SYMPTOMATIC_PERIOD [arg]`
- `--MEAN_HOSPITAL_REGULAR_PERIOD [arg]`
- `--MEAN_HOSPITAL_CRITICAL_PERIOD [arg]`

### City parameters

**Transmission coefficients**: The simulator uses some transmission coefficients in each of the interaction spaces to model the dynamics of the disease spread. They are set to some default values but they should be calibrated and provided via these options:

- `--BETA_H [arg]`: Transmission coefficient in the household
- `--BETA_C [arg]`: Transmission coefficient in the community
- `--BETA_W [arg]`: Transmission coefficient in the workplace
- `--BETA_S [arg]`: Transmission coefficient in the school
- `--BETA_TRAVEL [arg]`: Transmission coefficient in public transport
- `--BETA_CLASS [arg]`: Transmission coefficient in the ‘class’ subnetwork within school
- `--BETA_PROJECT [arg]`: Transmission coefficient in the ‘project’ subnetwork within workplace
- `--BETA_RANDOM_COMMUNITY [arg]`: Transmission coefficient in the “random community” (or “close friends”) subnetwork within community
- `--BETA_NBR_CELLS [arg]`: Transmission coefficient in the local neighbourhood cell (Note: This requires a few more options to be enabled for the neighbourhood cells to be initialised)

**Compliance**: For many of the intervention parameters, we also take into account the compliance of various households. Also, if the city explicitly contains HD areas, then the compliance in the HD and non-HD areas can be set separately via the following options:

- `--COMPLIANCE_PROBABILITY [arg]`: Compliance probability in the non-HD areas
- `--HD_COMPLIANCE_PROBABILITY [arg]`: Compliance probability in the HD areas

**Dynamics in HD areas**: The simulator also takes into account an increased spread in the HD areas via two parameters:

- `--HD_AREA_FACTOR [arg]`: A scaling factor for increasing the rate of infection in HD areas (default is 2)
- `--HD_AREA_EXPONENT [arg]`: An exponent to capture some non-linear “crowding” behaviour in HD areas (default is 0; recommend leaving it so)

**Other miscellaneous options**: In the simulator, there is a ‘distance kernel’ function that converts commute distances into a probability distribution. This function requires two parameters A and B; these can also be provided as options.

- `--F_KERNEL_A [arg]`
- `--F_KERNEL_B [arg]`

**Neighbourhood cells**: If you wish to enable some interaction at the level of neighbourhood cells, you need to explicitly enable this in the simulator. The simulator basically splits the city into a grid of small squares but you need to provide this bounding box. This is provided via the latitude/longitude of the south-west and the north-east corners.

- `--ENABLE_NBR_CELLS`: A boolean flag to enable initialisation of neighbourhood cells.
- `--CITY_SW_LAT [arg], --CITY_SW_LON [arg], --CITY_NE_LAT [arg], --CITY_SW_LON [arg]`: Latitude and longitudes of the south-west and north-east corners of the city.
- `--NBR_CELL_SIZE [arg]`: The side-length (in kms) of the neighbourhood cell to be initialised. The default value is 0.178 so that the area of each cell is roughly the area of a 100m circle.

**Cohorts**: `TBD`

### Intervention parameters

The simulator has some primitive interventions coded up and more sophisticated ones can be created by composing many of these primitive interventions. Some of the basic input parameters are the following:

- `--INTERVENTION [num]`: The index of the basic intervention you wish to simulate (e.g. 0 corresponds to no-intervention)
- `--CALIBRATION_DELAY [num]`: The number of days after which you wish to let the simulator start the intervention (the first num many days will be no-intervention)
- `--FIRST_PERIOD [num], --SECOND_PERIOD [num], --THIRD_PERIOD [num]`: Some intervention definitions allow the user to explicitly stay how long the first/second/third period is. This is provided using these options.

### Containment implementations

The simulator has two different forms of implementing containment zones. Both of them correspond to partial locking down a region depending on the number of hospitalised cases currently present in it. There is a “threshold” beyond which no additional containment is enforced, and the “leakage” specifies how accessible this region is beyond the threshold. The parameters are passed on to the simulator using the following options:

##### Community/ward-level containment

- `--ENABLE_CONTAINMENT`: A boolean flag to specific if containment zone implementations should be enabled
- `--LOCKED_COMMUNITY_LEAKAGE [arg]`: The lowest accessibility level of the ward/community (reached when fraction of hospitalisations are above the threshold)
- `--COMMUNITY_LOCK_THRESHOLD [arg]`: The fraction of hospitalisation in the ward/community beyond which the ward/community is locked to its maximum amount (which is specified by the leakage)

##### Neighbourhood-cell-level containment

- `--LOCKED_NEIGHBORHOOD_LEAKAGE [arg]`: The lowest accessibility level of the neighbourhood cell (reached when fraction of hospitalisations are above the threshold)
- `--NEIGHBORHOOD_LOCK_THRESHOLD [arg]`: The fraction of hospitalisation in the neighbourhood cell beyond which the neighbourhood cell is locked to its maximum amount (which is specified by the leakage)

##### Other options

- `--MASK_ACTIVE`: A boolean flag to include the use of masks of compliant households in the workplace and community networks
- `--MASK_START_DELAY [arg]`: When specified, agents start using masks from `CALIBRATION_DELAY + [arg]` many days of the simulator
- `--MASK_FACTOR [arg]`: The factor by which transmissions are dampened if the agent is wearing a mask

##### Interventions via json file

Interventions can also be specified via a json file. The exact format of the json file is described in details here. To use this, run the simulator with `--INTERVENTION 16` (as 16 corresponds to intervention loaded via json file) and use the following option to specify the file:

- `--intervention_filename [arg]`: The path (relative to the input directory) to the json file specifying the intervention

##### Attendance profiles

As a part of the intervention, you can also specify the attendance work profile via a json file. The format of the file is discussed in detail here. This can be specified to the simulator via the following options:

- `--IGNORE_ATTENDANCE_FILE`: A boolean flag explicitly asking the simulator to ignore the attendance file. The default value for this is True.
- `--attendance_file [arg]`: The path to the json file (relative to the input directory) with the attendance profile

###### Testing protocols

The simulator also has the capability to implement testing protocols. The testing protocol is also specified via a json file (details provided here).

- `--ENABLE_TESTING`: A boolean flag to tell the simulator to include testing protocols in the simulation
- `--testing_protocol_filename [arg]`: The path to the json file (relative to the input directory) with the testing protocol

**Note**: There are other command-line parameters of the simulator that I have not mentioned here as we do not use them (some are deprecated but are present for legacy reasons).

