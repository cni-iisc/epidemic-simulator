//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0

const WEBPAGE_VERSION = true;
//simulation inputs
const INPUTPATHPREFIX = 'staticInst/data/web_input_files';
const IMAGEPREFIX = 'simulator/images';
//var cityName = 'bengaluru';

NUM_DAYS = 120; //Number of days. Simulation duration
SIM_STEPS_PER_DAY = 4; //Number of simulation steps per day.
NUM_TIMESTEPS = NUM_DAYS * SIM_STEPS_PER_DAY; //
INIT_FRAC_INFECTED = 0.001; // Initial number of people infected
SEED_SCALING_FACTOR = 1.5;
SEED_WARD_FRACTION_UNIFORM = true
const RANDOM_SEEDING_WARDWISE = 0;
const SEED_FROM_INDIVIDUALS_JSON = 1;
const SEED_INFECTION_RATES = 2;
const SEED_EXP_RATE = 3;
SEEDING_MODE = RANDOM_SEEDING_WARDWISE;

// Seeding parameters for exp rate seeding
SEEDING_START_DATE = 0; // When to start seeding (with respect to simulator time)
SEEDING_DURATION = 22;  // For how long to seed (days) (March 1 - March 23)
SEEDING_DOUBLING_TIME = 4.18;	// Days after which the number of seeds double.
SEEDING_RATE_SCALE = 1;
CALIB_NO_INTERVENTION_DURATION = 24; // Lockdown starts from March 25
CALIB_LOCKDOWN_DURATION = 21;

//global variables.
NUM_PEOPLE = 100000; // Number of people. Will change once file is read.
NUM_HOMES = 25000; //Will change once file is read.
NUM_WORKPLACES = 5000; //Will change once file is read.
NUM_COMMUNITIES = 198; //Will change once file is read.
NUM_PUBLIC_TRANSPORT = 1;
NUM_SCHOOLS = 0;
NUM_DISEASE_STATES = 7; //0-S, 1-E, 2-I, 3-Symp,4-R, 5-H, 6-C, 7-D

var CASE_INFECTION_RATIO = 0.8; // Default value: only 80% of infections are detected as cases
var NUM_DETECTED_CASES = 0; // Track total detected cases

//Various interventions. These will need to be generalised soon.
CALIBRATION_DELAY = 0; //// Assuming the simulator starts on March 1.
NUM_DAYS_BEFORE_INTERVENTIONS = 24 + CALIBRATION_DELAY;
const CALIBRATION = -1
const NO_INTERVENTION = 0
const CASE_ISOLATION = 1
const HOME_QUARANTINE = 2
const LOCKDOWN = 3
const CASE_ISOLATION_AND_HOME_QUARANTINE = 4
const CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS = 5
const LOCKDOWN_40_CI_HQ_SD_70_PLUS_21_CI = 6
const LOCKDOWN_40 = 7
const LD_40_CI_HQ_SD70_SC_21_SC_42 = 8
const LD_40_CI_HQ_SD70_SC_21 = 9
const LD_40_CI_HQ_SD70_SC_OE_30 = 10
const HOME_QUARANTINE_DAYS = 14
const SELF_ISOLATION_DAYS = 7

INTERVENTION = NO_INTERVENTION; //run_and_plot() changes this

//Disease progression in an individual
const SUSCEPTIBLE = 0
const EXPOSED = 1
const PRE_SYMPTOMATIC = 2
const SYMPTOMATIC = 3
const RECOVERED = 4
const HOSPITALISED = 5
const CRITICAL = 6
const DEAD = 7

// Daily counters for tracking state transitions
let DAILY_NEW_EXPOSED = 0;// Number of new exposed individuals each day
let DAILY_NEW_PRESYMPTOMATIC = 0;// Number of new presymptomatic individuals each day
let DAILY_NEW_SYMPTOMATIC = 0;// Number of new symptomatic individuals each day
let DAILY_NEW_DETECTED = 0;// Number of new detected cases each day
let DAILY_NEW_HOSPITALISED = 0;// Number of new hospitalised individuals each day
let DAILY_NEW_CRITICAL = 0;// Number of new critical individuals each day
let DAILY_NEW_RECOVERED = 0;// Number of new recovered individuals each day
let DAILY_NEW_DEATHS = 0;// Number of new deaths each day

// Cumulative counters for tracking state transitions over the entire simulation
let CUMULATIVE_NEW_EXPOSED = 0;// Total number of exposed individuals over the simulation
let CUMULATIVE_NEW_PRESYMPTOMATIC = 0;// Total number of presymptomatic individuals over the simulation
let CUMULATIVE_NEW_SYMPTOMATIC = 0;// Total number of symptomatic individuals over the simulation
let CUMULATIVE_NEW_DETECTED = 0;// Total number of detected cases over the simulation
let CUMULATIVE_NEW_HOSPITALISED = 0;// Total number of hospitalised individuals over the simulation
let CUMULATIVE_NEW_CRITICAL = 0;// Total number of critical individuals over the simulation
let CUMULATIVE_NEW_RECOVERED = 0;// Total number of recovered individuals over the simulation
let CUMULATIVE_NEW_DEATHS = 0;// Total number of deaths over the simulation

// Additional global arrays to track daily counters over time for plotting
let days_daily_new_symptomatic = [];     // For cases plot daily Tarun
let days_cumulative_infected = [];       // For infected plot cumulative
let days_cumulative_hospitalised = [];   // For hospitalised plot cumulative
let days_cumulative_critical = [];       // For critical plot cumulative
let days_cumulative_deaths = [];         // For fatalities plot cumulative
let days_cumulative_recovered = [];      // For recovered plot cumulative
let days_affected_count = [];            // For cases plot cumulative
let days_daily_new_detected = [];         // For reported cases plot daily
let days_cumulative_detected = [];        // For reported cases plot cumulative

let csvContent = "data:text/csv;charset=utf-8,"; //for file dump

INCUBATION_PERIOD = 2.29
MEAN_ASYMPTOMATIC_PERIOD = 0.5
MEAN_SYMPTOMATIC_PERIOD = 5
MEAN_HOSPITAL_REGULAR_PERIOD = 8
MEAN_HOSPITAL_CRITICAL_PERIOD = 8
//These are parameters associated with the disease progression
const NUM_DAYS_TO_RECOG_SYMPTOMS = 1;
const INCUBATION_PERIOD_SHAPE = 2;

INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD * SIM_STEPS_PER_DAY; // 2.3 days
ASYMPTOMATIC_PERIOD = MEAN_ASYMPTOMATIC_PERIOD * SIM_STEPS_PER_DAY; // half a day
SYMPTOMATIC_PERIOD = MEAN_SYMPTOMATIC_PERIOD * SIM_STEPS_PER_DAY; // 5 days
HOSPITAL_REGULAR_PERIOD = MEAN_HOSPITAL_REGULAR_PERIOD * SIM_STEPS_PER_DAY;
HOSPITAL_CRITICAL_PERIOD = MEAN_HOSPITAL_CRITICAL_PERIOD * SIM_STEPS_PER_DAY;
SYMPTOMATIC_FRACTION = 0.67;
COMMUNITY_INFECTION_PROB = [];

const WTYPE_HOME = 0;
const WTYPE_OFFICE = 1;
const WTYPE_SCHOOL = 2;

NUM_AGE_GROUPS = 16;
USE_AGE_DEPENDENT_MIXING = false;

SIGMA_OFFICE = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
V_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_OFFICE = 16; //NUM_AGE_GROUPS;

SIGMA_SCHOOL = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
V_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_SCHOOL = 16; //NUM_AGE_GROUPS;

SIGMA_H = math.zeros([NUM_AGE_GROUPS]);
U_MATRIX_H = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
V_MATRIX_H = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
SIGNIFICANT_EIGEN_VALUES_H = 16; //NUM_AGE_GROUPS;

const PUBLIC_TRANSPORT_FRACTION = 0.5;
//age related transition probabilities, symptomatic to hospitalised to critical to fatality.
STATE_TRAN = [
    [0.0010000, 0.0500000, 0.4000000],
    [0.0030000, 0.0500000, 0.4000000],
    [0.0120000, 0.0500000, 0.5000000],
    [0.0320000, 0.0500000, 0.5000000],
    [0.0490000, 0.0630000, 0.5000000],
    [0.1020000, 0.1220000, 0.5000000],
    [0.1660000, 0.2740000, 0.5000000],
    [0.2430000, 0.4320000, 0.5000000],
    [0.2730000, 0.7090000, 0.5000000]
]

NUM_AFFECTED_COUNT = 0;

// Beta values
BETA_H = 0.67; //0.47 *1.0 //Thailand data
BETA_W = 0.50; //0.47 *1.0//Thailand data
BETA_S = 1.00; //0.94 *1.0//Thailand data
BETA_C = 0.15; //0.097*1// Thailand data. Product  = 0.47
BETA_PT = 0;
ALPHA = 0.8 //exponent of number of people in a household while normalising infection rate in a household.

// reset daily counters at the start of each day 
function reset_daily_counters() {
    DAILY_NEW_EXPOSED = 0;
    DAILY_NEW_PRESYMPTOMATIC = 0;
    DAILY_NEW_SYMPTOMATIC = 0;
    DAILY_NEW_RECOVERED = 0;
    DAILY_NEW_DETECTED = 0;
    DAILY_NEW_HOSPITALISED = 0;
    DAILY_NEW_CRITICAL = 0;
    DAILY_NEW_DEATHS = 0;
}

function clearSimulationMemory() {
    // Clear plot arrays
    days_daily_new_symptomatic = [];
    days_cumulative_infected = [];
    days_cumulative_hospitalised = [];
    days_cumulative_critical = [];
    days_cumulative_deaths = [];
    days_cumulative_recovered = [];
    days_affected_count = [];
    days_daily_new_detected = [];
    days_cumulative_detected = [];
    days_daily_new_deaths = [];
    days_daily_new_recovered = [];
    
    // Clear CSV strings (these can get very large)
    csvContent = "data:text/csv;charset=utf-8,";
    csvContent_alltogether = "data:text/csv;charset=utf-8,Day,Affected,Recovered,Infected,Exposed,Hospitalised,Critical,Fatalities,Lambda_H,Lambda_W,Lambda_C,Daily_New_Exposed,Daily_New_Presymptomatic,Daily_New_Symptomatic,Daily_New_Recovered,Daily_New_Detected,Daily_New_Hospitalised,Daily_New_Critical,Daily_New_Deaths,Cumulative_New_Exposed,Cumulative_New_Presymptomatic,Cumulative_New_Symptomatic,Cumulative_New_Recovered,Cumulative_New_Detected,Cumulative_New_Hospitalised,Cumulative_New_Critical,Cumulative_New_Deaths\r\n";
    
    // Clear lambda stats
    LAMBDA_INFECTION_STATS = [];
    LAMBDA_INFECTION_MEAN = [0, 0, 0, 0];
    
    // Clear plot data cache
    originalPlotData = {};
    
    // Reset counters
    NUM_AFFECTED_COUNT = 0;
    NUM_DETECTED_CASES = 0;
    reset_daily_counters();
    CUMULATIVE_NEW_EXPOSED = 0;
    CUMULATIVE_NEW_PRESYMPTOMATIC = 0;
    CUMULATIVE_NEW_SYMPTOMATIC = 0;
    CUMULATIVE_NEW_RECOVERED = 0;
    CUMULATIVE_NEW_DETECTED = 0;
    CUMULATIVE_NEW_HOSPITALISED = 0;
    CUMULATIVE_NEW_CRITICAL = 0;
    CUMULATIVE_NEW_DEATHS = 0;
}

// reset cumulative counters at the start of each day
function update_cumulative_counters() {
    CUMULATIVE_NEW_EXPOSED += DAILY_NEW_EXPOSED;
    CUMULATIVE_NEW_PRESYMPTOMATIC += DAILY_NEW_PRESYMPTOMATIC;
    CUMULATIVE_NEW_SYMPTOMATIC += DAILY_NEW_SYMPTOMATIC;
    CUMULATIVE_NEW_RECOVERED += DAILY_NEW_RECOVERED;
    CUMULATIVE_NEW_DETECTED += DAILY_NEW_DETECTED;
    CUMULATIVE_NEW_HOSPITALISED += DAILY_NEW_HOSPITALISED;
    CUMULATIVE_NEW_CRITICAL += DAILY_NEW_CRITICAL;
    CUMULATIVE_NEW_DEATHS += DAILY_NEW_DEATHS;
}

COMPLIANCE_PROBABILITY = set_compliance();

// Store the original data globally to always have correct reference
var originalPlotData = {};

//To what extent does a family comply with an intervention? 1 = full compliance, 0 = no compliance.
function set_compliance() {
    var val = 1;
    switch (INTERVENTION) {
        case CALIBRATION:
            val = 0.9; //No effect.
            break;
        case NO_INTERVENTION:
            val = 0.9; //No effect.
            break;
        case CASE_ISOLATION:
            val = 0.7;
            break;
        case HOME_QUARANTINE:
            val = 0.5;
            break;
        case LOCKDOWN:
            val = 0.9;
            break;
        case CASE_ISOLATION_AND_HOME_QUARANTINE:
            val = 0.7;
            break;
        case CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS:
            val = 0.7;
            break;
        case LOCKDOWN_40_CI_HQ_SD_70_PLUS_21_CI:
            val = 0.9;
            break;
        case LOCKDOWN_40:
            val = 0.9;
            break;
        case LD_40_CI_HQ_SD70_SC_21_SC_42:
            val = 0.9;
            break;
        case LD_40_CI_HQ_SD70_SC_21:
            val = 0.9;
            break;
        case LD_40_CI_HQ_SD70_SC_OE_30:
            val = 0.9;
            break;
        default:
            val = 1;
    }
    return val;
}

function compliance() {
    var val = (Math.random() < COMPLIANCE_PROBABILITY) ? 1 : 0;
    return val;
}

//This function seeds the infection based on ward-level probabilities.
function compute_prob_infection_given_community(infection_probability, set_uniform) {

    var prob_infec_given_community = []; // creating an array of probabilities for each community
    var communities_population_json = JSON.parse(loadJSON(inputPath + 'fractionPopulation.json'));
    if (!set_uniform) {
        var communities_frac_quarantined_json = JSON.parse(loadJSON(inputPath + 'quarantinedPopulation.json'));
    }

    var num_communities = communities_population_json.length;
    for (var w = 0; w < num_communities; w++) {
        if (SEEDING_MODE == RANDOM_SEEDING_WARDWISE) {
            if (set_uniform) {
                //set uniformly across wards. Ignore ward wise data.
                prob_infec_given_community.push(infection_probability);
            } else {
                //Use ward wise quarantine data
                prob_infec_given_community.push(infection_probability * communities_frac_quarantined_json[w]['fracQuarantined'] / communities_population_json[w]['fracPopulation']);

            }
        } else {
            prob_infec_given_community.push(0);
        }
    }
    return prob_infec_given_community;//return an array of probabilities for each community
}

function get_age_group(age) {
    var age_group = 0;
    age_group = Math.floor(age / 5);// // 0-4, 5-9, 10-14, 15-19, 20-24, 25-29, 30-34, 35-39, 40-44, 45-49, 50-54, 55-59, 60-64, 65-69, 70-74, 75-79
    if (age > 79) {
        age_group = 15;// 80+ age group
    }
    return age_group;
}

// Initialise the nodes with various features.
function init_nodes() {
    var individuals_json = JSON.parse(loadJSON(inputPath + 'individuals.json'));
    var workplace_json = JSON.parse(loadJSON(inputPath + 'workplaces.json'));
    //console.log(individuals_json.length,individuals_json[0]);
    NUM_PEOPLE = individuals_json.length;
    NUM_WORKPLACES = workplace_json.length;
    //console.log("Num People", NUM_PEOPLE, "Num Workspaces",NUM_WORKPLACES)
    if (SEEDING_MODE == RANDOM_SEEDING_WARDWISE) {
        COMMUNITY_INFECTION_PROB = compute_prob_infection_given_community(INIT_FRAC_INFECTED, SEED_WARD_FRACTION_UNIFORM);
    }
    //console.log(COMMUNITY_INFECTION_PROB)
    var nodes = [];//Array of nodes, each node is an individual in the population.
    var stream1 = new Random(1234);  //seed for random number generation. This is used to generate gamma distributed random numbers.

    for (var i = 0; i < NUM_PEOPLE; i++) {
        //console.log(stream1.gamma(1,1))
        var node = {
            'loc': [individuals_json[i]['lat'], individuals_json[i]['lon']], // [lat, long]
            'age': individuals_json[i]['age'],
            'age_group': get_age_group(individuals_json[i]['age']),
            'zeta_a': 1,
            'infectiousness': stream1.gamma(0.25, 4), // a.k.a. rho
            'severity': (Math.random() < 0.5) ? 1 : 0, // a.k.a. S_k
            'home': individuals_json[i]['household'],
            'workplace': individuals_json[i]['workplaceType'] == 1 ? individuals_json[i]['workplace'] : individuals_json[i]['school'],
            'community': individuals_json[i]['wardNo'] - 1, //minus one is temporary as the ward number indexing starts from 1,
            'time_of_infection': 0,
            'infection_status': 0, //
            'infective': 0,
            'lambda_h': 0, //individuals contribution to his home cluster
            'lambda_w': 0, //individuals contribution to his workplace cluster
            'lambda_c': 0, //individuals contribution to his community
            'lambda_pt': 0, //individuals contribution to his public transport
            'lambda': 0,
            'kappa_T': 1,
            'psi_T': 0,
            'public_transport': (Math.random() < PUBLIC_TRANSPORT_FRACTION) ? 0 : null,
            'funct_d_ck': f_kernel(individuals_json[i]['CommunityCentreDistance']), // TODO: need to use the kernel function. function of distance from community...f_kernel
            'dist_hw': 1,
            'workplace_type': individuals_json[i]['workplaceType'], //either school or office
            'lambda_incoming': [0, 0, 0, 0], //infectiousness from home, workplace, community as seen by individual
            'compliant': 1,
            'kappa_H': 1,
            'kappa_W': 1,
            'kappa_C': 1,
            'kappa_PT': 1,
            'incubation_period': stream1.gamma(INCUBATION_PERIOD_SHAPE, INCUBATION_PERIOD_SCALE),
            'asymptomatic_period': stream1.gamma(1, ASYMPTOMATIC_PERIOD),
            'symptomatic_period': stream1.gamma(1, SYMPTOMATIC_PERIOD),
            'hospital_regular_period': HOSPITAL_REGULAR_PERIOD,
            'hospital_critical_period': HOSPITAL_CRITICAL_PERIOD,
            'kappa_H_incoming': 1,
            'kappa_W_incoming': 1,
            'kappa_C_incoming': 1,
            'kappa_PT_incoming': 1,
            'quarantined': false
        };


        //Correct initialisation for individuals not associated to workplace or school
        if (node['workplace_type'] == WTYPE_HOME) {
            node['workplace'] = null;
            node['public_transport'] = null;
        }
        if (node['workplace'] == null) {
            //A safety check in case workplace is null but workplace type is not home.
            node['workplace_type'] = WTYPE_HOME;
            node['public_transport'] = null;
        }

        //Set infective status, set the time of infection, and other age-related factors
        node['infective'] = node['infection_status'] == PRE_SYMPTOMATIC ? 1 : 0; //initialise all infected individuals as infective
        node['time_of_infection'] = node['infection_status'] == EXPOSED ? (-node['incubation_period'] * Math.random()) : 0;
        node['zeta_a'] = zeta(node['age']);

        if (SEEDING_MODE == SEED_FROM_INDIVIDUALS_JSON) {
            node['infection_status'] = individuals_json[i]['infection_status'];
            node['time_of_infection'] = node['infection_status'] == EXPOSED ? (individuals_json[i]['time_of_infection'] * SIM_STEPS_PER_DAY - node['incubation_period']) : 0;
            
        } else if (SEEDING_MODE == RANDOM_SEEDING_WARDWISE) {
            node['infection_status'] = (Math.random() < COMMUNITY_INFECTION_PROB[individuals_json[i]['wardNo'] - 1]) ? 1 : 0
        }
        nodes.push(node)

    }
    return nodes;
}

function load_infection_seed_json(seed_scaling_factor) {
    var seed_array = [];
    var infection_seeds_json = JSON.parse(loadJSON(inputPath + 'infection_seeds.json'));
    var infection_seeds_json_array = Object.values(infection_seeds_json['seed_fit']);
    var num_seed_days = infection_seeds_json_array.length;
    for (var count = 0; count < num_seed_days * SIM_STEPS_PER_DAY; count++) {
        var mean = (infection_seeds_json_array[Math.floor(count / SIM_STEPS_PER_DAY)] / SIM_STEPS_PER_DAY) * seed_scaling_factor;
        seed_array.push(d3.randomPoisson(mean)());
    }
    return seed_array;
}

function infection_seeding(nodes, seed_count, curr_time) {
    var num_individuals_to_seed = seed_count;
    var num_seeded = 0;
    while (num_seeded < num_individuals_to_seed) {
        let individual_index = d3.randomInt(0, NUM_PEOPLE)();
        if (nodes[individual_index]['infection_status'] == SUSCEPTIBLE) {
            nodes[individual_index]['infection_status'] = EXPOSED;
            nodes[individual_index]['time_of_infection'] = curr_time; 
            num_seeded++;
        }
    }
    // console.log(curr_time / SIM_STEPS_PER_DAY, num_individuals_to_seed);
}

function infection_seeding_exp_rate(nodes, curr_time) {
    if (curr_time >= SEEDING_START_DATE * SIM_STEPS_PER_DAY && curr_time < (SEEDING_START_DATE + SEEDING_DURATION) * SIM_STEPS_PER_DAY) {
        var time_since_seeding_start = curr_time - SEEDING_START_DATE * SIM_STEPS_PER_DAY;
        var seed_doubling_time = SEEDING_DOUBLING_TIME * SIM_STEPS_PER_DAY;
        var current_seeding_rate = SEEDING_RATE_SCALE * Math.pow(2, time_since_seeding_start / seed_doubling_time);
        var num_seeds_curr_time = d3.randomPoisson(current_seeding_rate)();
        var num_seeded = 0;
        // console.log(curr_time, num_seeds_curr_time);
        while (num_seeded < num_seeds_curr_time) {
            let individual_index = d3.randomInt(0, NUM_PEOPLE)();
            if (nodes[individual_index]['infection_status'] == SUSCEPTIBLE) {
                nodes[individual_index]['infection_status'] = EXPOSED;
                nodes[individual_index]['time_of_infection'] = curr_time; 
                num_seeded++;
            }
        }
    }
}

// This is a multiplication factor that quantifies an individual's infective status given the infection state.
function kappa_T(node, cur_time) {
    var val = 0;
    if (node["infective"] != 1) {
        val = 0;
    } else {
        var time_since_infection = cur_time - node["time_of_infection"];

        if (time_since_infection < node['incubation_period'] || time_since_infection > (node['incubation_period'] + node['asymptomatic_period'] + node['symptomatic_period'])) {
            // Individual is not yet symptomatic or has been recovered, or has moved to the hospital
            val = 0;
        } else if (time_since_infection < node['incubation_period'] + node['asymptomatic_period']) {
            val = 1;
        } else {
            val = 1.5;
        }
    }
    return val;
}

// Absenteeism parameter. This may depend on the workplace type.
function psi_T(node, cur_time) {
    if (node["infective"] != 1) { //check if not infectious
        return 0;
    }
    var PSI_THRESHOLD = SIM_STEPS_PER_DAY;
    var time_since_infection = cur_time - node["time_of_infection"];
    var scale_factor = 0.5;
    if (node['workplace_type'] == WTYPE_SCHOOL) {
        scale_factor = 0.1
    } //school
    else if (node['workplace_type'] == WTYPE_OFFICE) {
        scale_factor = 0.5
    } //office
    if (time_since_infection < PSI_THRESHOLD) {
        return 0;
    } else {
        return scale_factor;
    }
}

function f_kernel(d) {
    var a = 10.751 //4. in kms
    var b = 5.384 //3.8. both values are for Thailand, until we get a fit for India
    return 1 / (1 + Math.pow(d / a, b))

}

function zeta(age) {
    // This might change based on better age-related interaction data.
    if (age < 5) {
        return 0.1;
    } else if (age < 10) {
        return 0.25;
    } else if (age < 15) {
        return 0.5;
    } else if (age < 20) {
        return 0.75;
    } else if (age < 65) {
        return 1;
    } else if (age < 70) {
        return 0.75;
    } else if (age < 75) {
        return 0.5;
    } else if (age < 85) {
        return 0.25;
    } else {
        return 0.1;
    }
}

// Compute scale factors for each home, workplace and community. Done once at the beginning.
function compute_scale_homes(homes) {

    for (var w = 0; w < homes.length; w++) {
        if (homes[w]['individuals'].length == 0) {
            homes[w]['scale'] = 0;
        } else {
            homes[w]['scale'] = BETA_H * homes[w]['Q_h'] / (Math.pow(homes[w]['individuals'].length, ALPHA));
        }
    }
}

function compute_scale_workplaces(workplaces) {
    var beta_workplace
    for (var w = 0; w < workplaces.length; w++) {
        if (workplaces[w]['individuals'].length == 0) {
            workplaces[w]['scale'] = 0
        } else {
            if (workplaces[w]['workplace_type'] == WTYPE_OFFICE) {
                beta_workplace = BETA_W; //workplace
            } else if (workplaces[w]['workplace_type'] == WTYPE_SCHOOL) {
                beta_workplace = BETA_S; //school
            }
            workplaces[w]['scale'] = beta_workplace * workplaces[w]['Q_w'] / workplaces[w]['individuals'].length;
        }
    }
}

function compute_scale_communities(nodes, communities) {

    for (var w = 0; w < communities.length; w++) {

        var sum_value = 0;
        for (var i = 0; i < communities[w]['individuals'].length; i++) {
            sum_value += nodes[communities[w]['individuals'][i]]['funct_d_ck'];
        }
        if (sum_value == 0) {
            communities[w]['scale'] = 0;
        } else communities[w]['scale'] = BETA_C * communities[w]['Q_c'] / sum_value;
    }

}

function compute_scale_public_transport(nodes, public_transports) {

    for (var tr = 0; tr < public_transports.length; tr++) {

        var sum_value = 0;
        for (var i = 0; i < public_transports[tr]['individuals'].length; i++) {
            sum_value += nodes[public_transports[tr]['individuals'][i]]['dist_hw'];
        }
        if (sum_value == 0) {
            public_transports[tr]['scale'] = 0;
        } else {
            public_transports[tr]['scale'] = BETA_PT * public_transports[tr]['Q_c'] / sum_value;

        }
        // console.log(tr, public_transports[tr]['scale']);
    }

}

function init_homes() {

    var houses_json = JSON.parse(loadJSON(inputPath + 'houses.json'));
    NUM_HOMES = houses_json.length;

    var homes = [];
    for (var h = 0; h < NUM_HOMES; h++) {
        var home = {
            'loc': [houses_json[h]['lat'], houses_json[h]['lon']], // [lat, long],
            'lambda_home': 0,
            'individuals': [], // We will populate this later
            'Q_h': 1,
            'scale': 0,
            'compliant': compliance(),
            'quarantined': false,
            'age_dependent_mixing': math.zeros([NUM_AGE_GROUPS])
        };        
        homes.push(home)

    }
    if (USE_AGE_DEPENDENT_MIXING) {
        get_age_dependent_mixing_matrix_household(); //get age dependent mixing matrix for households.
    }
    return homes;
}

// Both schools and offices are treated as workspaces with some differences
function init_workplaces() {
    var workplaces_json = JSON.parse(loadJSON(inputPath + 'workplaces.json'));
    var schools_json = JSON.parse(loadJSON(inputPath + 'schools.json'));
    NUM_WORKPLACES = workplaces_json.length;
    NUM_SCHOOLS = schools_json.length;
    // console.log(NUM_WORKPLACES,NUM_SCHOOLS)
    var workplaces = [];
    // schools come first followed by workspaces

    for (var w = 0; w < NUM_SCHOOLS; w++) {
        var workplace = {
            'loc': [schools_json[w]['lat'], schools_json[w]['lon']], // [lat, long],
            'lambda_workplace': 0,
            'individuals': [], //get_individuals_at_workplace(nodes, w), // Populate with individuals in same workplace
            'Q_w': 1,
            'scale': 0,
            'workplace_type': WTYPE_SCHOOL, //tells this workplace is a school
            'quarantined': false,
            'age_dependent_mixing': math.zeros([NUM_AGE_GROUPS])
        };
        workplaces.push(workplace)
    }

    for (var w = 0; w < NUM_WORKPLACES; w++) {
        var workplace = {
            'loc': [workplaces_json[w]['lat'], workplaces_json[w]['lon']], // [lat, long],
            'lambda_workplace': 0,
            'individuals': [], //get_individuals_at_workplace(nodes, w), // Populate with individuals in same workplace
            'Q_w': 1,
            'scale': 0,
            'workplace_type': WTYPE_OFFICE,//tells this workplace is an office
            'quarantined': false,
            'age_dependent_mixing': math.zeros([NUM_AGE_GROUPS])
        };
        workplaces.push(workplace)
    }
    if (USE_AGE_DEPENDENT_MIXING) {
        get_age_dependent_mixing_matrix_workplace();
    }

    return workplaces;
}

function compare_wards(a, b) {
    // Function to sort wards
    const wardA = a["wardNo"];
    const wardB = b["wardNo"];

    let comparison = 0;
    if (wardA > wardB) {
        comparison = 1;
    } else if (wardA < wardB) {
        comparison = -1;
    }
    return comparison;
}

function init_community() {
    var communities_json = JSON.parse(loadJSON(inputPath + 'commonArea.json'));
    communities_json.sort(compare_wards);
    NUM_COMMUNITIES = communities_json.length;
    var communities = [];
    for (var c = 0; c < NUM_COMMUNITIES; c++) {
        var community = {
            'loc': [communities_json[c]['lat'], communities_json[c]['lon']], // [lat, long]
            'lambda_community': 0,
            'lambda_community_global': 0,
            'individuals': [], // We will populate this later
            'Q_c': 1,
            'scale': 0,
            'quarantined': false
        };

        communities.push(community)
    }
    return communities;
}

function init_public_transport() {
    var public_transport_entities = [];
    for (var pt = 0; pt < NUM_PUBLIC_TRANSPORT; pt++) {
        var public_transport_entity = {
            'loc': [0, 0], // [lat, long]
            'lambda_PT': 0,
            'individuals': [], // We will populate this later
            'Q_c': 1,
            'scale': 0,
            'quarantined': false
        };

        public_transport_entities.push(public_transport_entity)
    }
    return public_transport_entities;
}


const RADIUS_EARTH = 6371; ///km
function euclidean(loc1, loc2) {
    var lat1 = loc1[0];
    var lon1 = loc1[1];
    var lat2 = loc2[0];
    var lon2 = loc2[1];

    if ((lat1 == lat2) && (lon1 == lon2)) {
        return 0;
    } else {

        var radlat1 = Math.PI * lat1 / 180;
        var radlat2 = Math.PI * lat2 / 180;
        var theta = lon1 - lon2;
        var radtheta = Math.PI * theta / 180;
        var dist = 1 - (Math.sin(radlat1) * Math.sin(radlat2) + Math.cos(radlat1) * Math.cos(radlat2) * Math.cos(radtheta));
        if (dist < 0) {
            dist = 0;
        }
        dist = Math.sqrt(2 * dist);
        dist = dist * RADIUS_EARTH;
        return dist;
    }
}


function compute_community_distances(communities) {
    var inter_ward_distances_json = JSON.parse(loadJSON(inputPath + 'wardCentreDistance.json'));

    var community_dist_matrix = math.zeros([communities.length, communities.length]);
    /// console.log(community_dist_matrix)
    for (var c1 = 0; c1 < inter_ward_distances_json.length; c1++) {
        for (var c2 = c1 + 1; c2 < inter_ward_distances_json.length; c2++) {
            
            community_dist_matrix[c1][c2] = inter_ward_distances_json[c1][(c2 + 1).toString()];
            community_dist_matrix[c2][c1] = community_dist_matrix[c1][c2];

        }
    }

    return community_dist_matrix;
}

function compute_individual_community_distance(nodes, communities) {
    //Assign individuals to homes, workplace, community
    for (var i = 0; i < nodes.length; i++) {
        nodes[i]['funct_d_ck'] = f_kernel(euclidean(nodes[i]['loc'], communities[nodes[i]['community']]['loc']));
    }
}


function assign_individual_home_community(nodes, homes, workplaces, communities) {
    //Assign individuals to homes, workplace, community
    for (var i = 0; i < nodes.length; i++) {
        if (nodes[i]['home'] != null) {

            homes[nodes[i]['home']]['individuals'].push(i); //No checking for null as all individuals have a home
            nodes[i]['compliant'] = homes[nodes[i]['home']]['compliant']; //All members of the household are set the same compliance value
        }
        if (nodes[i]['workplace'] != null) {
            workplaces[nodes[i]['workplace']]['individuals'].push(i);
        }
        if (nodes[i]['community'] != null) {
            communities[nodes[i]['community']]['individuals'].push(i);
        }
    }

}

function assign_individual_public_transports(nodes, public_transports, workplaces) {
    //Assign individuals to homes, workplace, community
    for (var i = 0; i < nodes.length; i++) {
        if (nodes[i]['public_transport'] != null && (nodes[i]['workplace_type'] == WTYPE_OFFICE || nodes[i]['workplace_type'] == WTYPE_SCHOOL)) {
            public_transports[nodes[i]['public_transport']]['individuals'].push(i); //No checking for null as all individuals have a home
            nodes[i]['dist_hw'] = euclidean(nodes[i]['loc'], workplaces[nodes[i]['workplace']]['loc']);
        }
    }
}

function update_individual_lambda_h(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * (1 + node['severity']) * node['kappa_H'];
}

function update_individual_lambda_w(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * (1 + node['severity'] * (2 * node['psi_T'] - 1)) * node['kappa_W'];
}

function update_individual_lambda_c(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * node['funct_d_ck'] * (1 + node['severity']) * node['kappa_C'];
    
}

function update_individual_lambda_pt(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * node['dist_hw'] * (1 + node['severity'] * (2 * node['psi_T'] - 1)) * node['kappa_PT'];
    
}

function get_init_stats(nodes, homes, workplaces, communities) {
    for (var h = 0; h < homes.length; h++) {
        let working_count = 0
        for (var ind_count = 0; ind_count < homes[h]['individuals'].length; ind_count++) {
            if (nodes[homes[h]['individuals'][ind_count]]['workplace_type'] == 1) working_count++;
        }
        // console.log("Home: ", h, " - ", homes[h]['individuals'].length, ". Working individuals  = ", working_count);
    }
    for (var h = 0; h < workplaces.length; h++) {
        // console.log("workplace: ", h, " - ", workplaces[h]['individuals'].length)
    }
    for (var h = 0; h < communities.length; h++) {
        // console.log("Community: ", h, " - ", communities[h]['individuals'].length)
    }
}

function update_infection(node, cur_time) {
    var age_index = 0;
    // Determine age category of individual. TODO: Could be part of individual datastructure as this is static
    if (node['age'] < 10) {
        age_index = 0;
    } else if (node['age'] < 20) {
        age_index = 1;
    } else if (node['age'] < 30) {
        age_index = 2;
    } else if (node['age'] < 40) {
        age_index = 3;
    } else if (node['age'] < 50) {
        age_index = 4;
    } else if (node['age'] < 60) {
        age_index = 5;
    } else if (node['age'] < 70) {
        age_index = 6;
    } else if (node['age'] < 80) {
        age_index = 7;
    } else {
        age_index = 8;
    }

    // SUSCEPTIBLE → EXPOSED
    if (node['infection_status'] == SUSCEPTIBLE && Math.random() < (1 - Math.exp(-node['lambda'] / SIM_STEPS_PER_DAY))) {
        node['infection_status'] = EXPOSED; //move to exposed state
        node['time_of_infection'] = cur_time;
        node['infective'] = 0;
        update_lambda_stats(node);
        DAILY_NEW_EXPOSED++;  // Track Daily new exposure 
    } 
    // EXPOSED → PRE_SYMPTOMATIC
    else if (node['infection_status'] == EXPOSED && (cur_time - node['time_of_infection'] >= node['incubation_period'])) {
        node['infection_status'] = PRE_SYMPTOMATIC;
        node['infective'] = 1;
        DAILY_NEW_PRESYMPTOMATIC++;  // Track Daily new pre-symptomatic 
    } 
    // PRE_SYMPTOMATIC → SYMPTOMATIC or RECOVERED
    else if (node['infection_status'] == PRE_SYMPTOMATIC && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period']))) {
        if (Math.random() < SYMPTOMATIC_FRACTION) {
            // Person becomes symptomatic
            node['infection_status'] = SYMPTOMATIC;
            node['infective'] = 1;
            NUM_AFFECTED_COUNT++;
            DAILY_NEW_SYMPTOMATIC++;
            
            if (Math.random() < CASE_INFECTION_RATIO) {
                node['detected'] = true;
                NUM_DETECTED_CASES++;
                DAILY_NEW_DETECTED++;
            }
            
        } else {
            // Person is asymptomatic - they recover without symptoms
            node['infection_status'] = RECOVERED;
            node['infective'] = 0;
            DAILY_NEW_RECOVERED++;
        }
    } 
    // SYMPTOMATIC → HOSPITALISED or RECOVERED
    else if (node['infection_status'] == SYMPTOMATIC && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period'] + node['symptomatic_period']))) {
        
        // Determine if person needs hospitalization
        var needs_hospitalization = Math.random() < STATE_TRAN[age_index][0];
        
        if (needs_hospitalization) {
            // Person gets hospitalized
            node['infection_status'] = HOSPITALISED;
            node['infective'] = 0;
            DAILY_NEW_HOSPITALISED++;
            
            // MODIFIED: Only detect if not already detected
            if (!node['detected']) {
                node['detected'] = true;
                NUM_DETECTED_CASES++;
                DAILY_NEW_DETECTED++;
            }
        } else {
            // Person recovers
            node['infection_status'] = RECOVERED;
            node['infective'] = 0;
            DAILY_NEW_RECOVERED++;
        }
    }
    // HOSPITALISED → CRITICAL or RECOVERED
    else if (node['infection_status'] == HOSPITALISED && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period'] + node['symptomatic_period'] + node['hospital_regular_period']))) {
        // All hospitalized people are already detected and counted
        if (Math.random() < STATE_TRAN[age_index][1]) {
            node['infection_status'] = CRITICAL;
            node['infective'] = 0;
            DAILY_NEW_CRITICAL++;
        } else {
            node['infection_status'] = RECOVERED;
            node['infective'] = 0;
            DAILY_NEW_RECOVERED++;
        }
    } 
    // CRITICAL → DEAD or RECOVERED
    else if (node['infection_status'] == CRITICAL && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period'] + node['symptomatic_period'] + node['hospital_regular_period'] + node['hospital_critical_period']))) {
        // All critical people are already detected and counted
        if (Math.random() < STATE_TRAN[age_index][2]) {
            node['infection_status'] = DEAD;
            node['infective'] = 0;
            DAILY_NEW_DEATHS++;
        } else {
            node['infection_status'] = RECOVERED;
            node['infective'] = 0;
            DAILY_NEW_RECOVERED++;
        }
    }

    node['lambda_h'] = update_individual_lambda_h(node);
    node['lambda_w'] = update_individual_lambda_w(node);
    node['lambda_c'] = update_individual_lambda_c(node);
    node['lambda_pt'] = update_individual_lambda_pt(node);
}

function update_psi(node, cur_time) {
    node['psi_T'] = psi_T(node, cur_time);
    //console.log(node['psi_T'])
}

function update_lambda_h_old(nodes, home) {
    var sum_value = 0

    for (var i = 0; i < home['individuals'].length; i++) {
        sum_value += nodes[home['individuals'][i]]['lambda_h'];
    }
    return home['scale'] * sum_value;
}

function get_age_dependent_mixing_matrix_household() {
    var sigma_json = JSON.parse(loadJSON(inputPath + 'Sigma_household.json'));
    var U_matrix_json = JSON.parse(loadJSON(inputPath + 'U_household.json'));
    var V_matrix_json = JSON.parse(loadJSON(inputPath + 'Vtranspose_household.json'));

    SIGMA_H = math.zeros([NUM_AGE_GROUPS]);
    U_MATRIX_H = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
    V_MATRIX_H = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);

    for (var count = 0; count < NUM_AGE_GROUPS; count++) {
        //sigma_json is read as a diagonal matrix.
        SIGMA_H[count] = sigma_json[count][count];
    }
    for (var count = 0; count < NUM_AGE_GROUPS; count++) {
        for (var count2 = 0; count2 < NUM_AGE_GROUPS; count2++) {
            U_MATRIX_H[count][count2] = U_matrix_json[count2][count]; //After JSON parsing, what you get is the transposed version.
            V_MATRIX_H[count][count2] = V_matrix_json[count2][count]; //V_MATRIX is the transpose of V in C = SUV'.
        }
    }
}

function update_lambda_h(nodes, home) {
    //Compute age_group related mixing
    var lambda_age_group = math.zeros([NUM_AGE_GROUPS]);

    if (!USE_AGE_DEPENDENT_MIXING) {
        
        var lambda_old = update_lambda_h_old(nodes, home)


        for (var count = 0; count < NUM_AGE_GROUPS; count++) {
            lambda_age_group[count] = lambda_old;
        }
    } else {
        var SIGMA = SIGMA_H;
        var V_MATRIX = V_MATRIX_H;
        var U_MATRIX = U_MATRIX_H;
        var SIGNIFICANT_EIGEN_VALUES = SIGNIFICANT_EIGEN_VALUES_H

        //add contributions to each age group
        var age_components = math.zeros([NUM_AGE_GROUPS]);
        for (var indv_count = 0; indv_count < home['individuals'].length; indv_count++) {
            var indv_age_group = nodes[home['individuals'][indv_count]]['age_group'];
            age_components[indv_age_group] += nodes[home['individuals'][indv_count]]['lambda_h'];
        }
        //weighted sum of age contributions for each eigen component
        var V_T_x = math.zeros([SIGNIFICANT_EIGEN_VALUES]);
        for (var eigen_count = 0; eigen_count < SIGNIFICANT_EIGEN_VALUES; eigen_count++) {
            for (var count = 0; count < NUM_AGE_GROUPS; count++) {
                V_T_x[eigen_count] += V_MATRIX[eigen_count][count] * age_components[count];//Assumption is V_matrix is V' where C = USV'
            }
        }

        for (var count = 0; count < NUM_AGE_GROUPS; count++) {
            for (var eigen_count = 0; eigen_count < SIGNIFICANT_EIGEN_VALUES; eigen_count++) {
                lambda_age_group[count] += home['scale'] * SIGMA[eigen_count] * U_MATRIX[count][eigen_count] * V_T_x[eigen_count];
            }
        }
    }
    return lambda_age_group;
}

function update_lambda_w_old(nodes, workplace) {
    var sum_value = 0

    for (var i = 0; i < workplace['individuals'].length; i++) {
        
        sum_value += nodes[workplace['individuals'][i]]['lambda_w'];
    }
    return workplace['scale'] * sum_value;
    // Populate it afterwards...
}

function get_age_dependent_mixing_matrix_workplace() {
    var sigma_json = JSON.parse(loadJSON(inputPath + 'Sigma_workplace.json'));
    var U_matrix_json = JSON.parse(loadJSON(inputPath + 'U_workplace.json'));
    var V_matrix_json = JSON.parse(loadJSON(inputPath + 'Vtranspose_workplace.json'));

    SIGMA_OFFICE = math.zeros([NUM_AGE_GROUPS]);
    U_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
    V_MATRIX_OFFICE = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);

    for (var count = 0; count < NUM_AGE_GROUPS; count++) {
        //sigma_json is read as a diagonal matrix.
        SIGMA_OFFICE[count] = sigma_json[count][count];
    }
    for (var count = 0; count < NUM_AGE_GROUPS; count++) {
        for (var count2 = 0; count2 < NUM_AGE_GROUPS; count2++) {
            U_MATRIX_OFFICE[count][count2] = U_matrix_json[count2][count]; //After JSON parsing, what you get is the transposed version.
            V_MATRIX_OFFICE[count][count2] = V_matrix_json[count2][count]; //V_MATRIX is the transpose of V in C = SUV'.
        }

    }

    var sigma_json = JSON.parse(loadJSON(inputPath + 'Sigma_school.json'));
    var U_matrix_json = JSON.parse(loadJSON(inputPath + 'U_school.json'));
    var V_matrix_json = JSON.parse(loadJSON(inputPath + 'Vtranspose_school.json'));

    SIGMA_SCHOOL = math.zeros([NUM_AGE_GROUPS]);
    U_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);
    V_MATRIX_SCHOOL = math.zeros([NUM_AGE_GROUPS, NUM_AGE_GROUPS]);

    for (var count = 0; count < NUM_AGE_GROUPS; count++) { //TODO:Change loop max to NUM_AGE_GROUPS
        SIGMA_SCHOOL[count] = sigma_json[count][count];
    }
    for (var count = 0; count < NUM_AGE_GROUPS; count++) {
        for (var count2 = 0; count2 < NUM_AGE_GROUPS; count2++) {
            U_MATRIX_SCHOOL[count][count2] = U_matrix_json[count2][count];
            V_MATRIX_SCHOOL[count][count2] = V_matrix_json[count2][count];
        }

    }
}

function update_lambda_w(nodes, workplace) {
    //Compute age_group related mixing
    var lambda_age_group = math.zeros([NUM_AGE_GROUPS]);

    if (!USE_AGE_DEPENDENT_MIXING) {
        
        var lambda_old = update_lambda_w_old(nodes, workplace)
        for (var count = 0; count < NUM_AGE_GROUPS; count++) {
            lambda_age_group[count] = lambda_old;
        }
       
    } else {

        var SIGMA = SIGMA_OFFICE;
        var V_MATRIX = V_MATRIX_OFFICE;
        var U_MATRIX = U_MATRIX_OFFICE;
        var SIGNIFICANT_EIGEN_VALUES = SIGNIFICANT_EIGEN_VALUES_OFFICE

        if (workplace['workplace_type'] == WTYPE_SCHOOL) {
            SIGMA = SIGMA_SCHOOL;
            V_MATRIX = V_MATRIX_SCHOOL;
            U_MATRIX = U_MATRIX_SCHOOL;
            SIGNIFICANT_EIGEN_VALUES = SIGNIFICANT_EIGEN_VALUES_SCHOOL
        }
        //add contributions to each age group
        var age_components = math.zeros([NUM_AGE_GROUPS]);
        for (var student_count = 0; student_count < workplace['individuals'].length; student_count++) {
            var student_age_group = nodes[workplace['individuals'][student_count]]['age_group'];
            age_components[student_age_group] += nodes[workplace['individuals'][student_count]]['lambda_w'];
        }
        //weighted sum of age contributions for each eigen component
        var V_T_x = math.zeros([SIGNIFICANT_EIGEN_VALUES]);
        for (var eigen_count = 0; eigen_count < SIGNIFICANT_EIGEN_VALUES; eigen_count++) {
            for (var count = 0; count < NUM_AGE_GROUPS; count++) {
                V_T_x[eigen_count] += V_MATRIX[eigen_count][count] * age_components[count];//Assumption is V_matrix is V' where C = USV'
            }
        }
        for (var count = 0; count < NUM_AGE_GROUPS; count++) {
            for (var eigen_count = 0; eigen_count < SIGNIFICANT_EIGEN_VALUES; eigen_count++) {
                lambda_age_group[count] += workplace['scale'] * SIGMA[eigen_count] * U_MATRIX[count][eigen_count] * V_T_x[eigen_count];
            }
        }
    }
    return lambda_age_group;
}

function update_lambda_public_transport(nodes, public_transport) {
    var sum_value = 0
    for (var i = 0; i < public_transport['individuals'].length; i++) {
        
        sum_value += nodes[public_transport['individuals'][i]]['lambda_pt'];
    }
    return public_transport['scale'] * sum_value;
    
}

function update_lambda_c_local(nodes, community) {
    var sum_value = 0
    for (var i = 0; i < community['individuals'].length; i++) {
        
        sum_value += nodes[community['individuals'][i]]['lambda_c'];
    }
    return community['scale'] * sum_value;
    
}

function update_lambda_c_global(communities, community_distance_matrix) {
    for (var c1 = 0; c1 < communities.length; c1++) {
        var temp = 0;
        var temp2 = 0;
        for (var c2 = 0; c2 < communities.length; c2++) {
            temp += f_kernel(community_distance_matrix[c1][c2]) * communities[c2]['lambda_community'];
            temp2 += f_kernel(community_distance_matrix[c1][c2]);
            //console.log(c1,c2,f_kernel(community_distance_matrix[c1][c2])*communities[c2]['lambda_community'])
        }
        communities[c1]['lambda_community_global'] = temp / temp2;
    }
}
// responcible for getting the infected individuals in a community
function get_infected_community(nodes, community) {
    var affected_stat = 0
    var infected_stat = 0
    var exposed_stat = 0
    var hospitalised_stat = 0
    var critical_stat = 0
    var dead_stat = 0
    var recovered_stat = 0
    var exposed_stat = 0
    for (var i = 0; i < community['individuals'].length; i++) {
        
        if (nodes[community['individuals'][i]]['infection_status'] == EXPOSED) {
            exposed_stat += 1
        }
        if (nodes[community['individuals'][i]]['infection_status'] == RECOVERED) {
            recovered_stat += 1
        }
        if (nodes[community['individuals'][i]]['infection_status'] == HOSPITALISED) {
            hospitalised_stat += 1
        }
        if (nodes[community['individuals'][i]]['infection_status'] == CRITICAL) {
            critical_stat += 1
        }
        if (nodes[community['individuals'][i]]['infection_status'] == DEAD) {
            dead_stat += 1
        }
        if (nodes[community['individuals'][i]]['infection_status'] == PRE_SYMPTOMATIC ||
            nodes[community['individuals'][i]]['infection_status'] == SYMPTOMATIC ||
            nodes[community['individuals'][i]]['infection_status'] == HOSPITALISED ||
            nodes[community['individuals'][i]]['infection_status'] == CRITICAL) {
            infected_stat += 1
        }
    }
    affected_stat = exposed_stat + infected_stat + recovered_stat + dead_stat;
    return [infected_stat, affected_stat, hospitalised_stat, critical_stat, dead_stat];
    
}

function update_lambdas(node, homes, workplaces, communities, public_transports, nodes, cur_time) {
    node['lambda_incoming'] = [0, 0, 0, 0];
    if (node['home'] != null && node['home'] != undefined) {
        var age_group = node['age_group'];
        node['lambda_incoming'][0] = node['kappa_H_incoming'] * homes[node['home']]['age_dependent_mixing'][age_group];
    }
    if (node['workplace'] != null && node['workplace'] != undefined) {
        var age_group = node['age_group'];
        node['lambda_incoming'][1] = node['kappa_W_incoming'] * workplaces[node['workplace']]['age_dependent_mixing'][age_group];
    }
    if (node['community'] != null && node['community'] != undefined) {
        //for all communities
        //add the community lambda with a distance related scaling factor
        node['lambda_incoming'][2] = node['kappa_C_incoming'] * node['zeta_a'] * node['funct_d_ck'] * communities[node['community']]['lambda_community_global'];
    }
    if (node['public_transport'] != null && node['public_transport'] != undefined) {
        //for all public transport
        //console.log("In lambda incoming.")
        node['lambda_incoming'][3] = node['kappa_PT_incoming'] * node['dist_hw'] * public_transports[node['public_transport']]['lambda_PT'];
    }
    node['lambda'] = node['lambda_incoming'][0] + node['lambda_incoming'][1] + node['lambda_incoming'][2] + node['lambda_incoming'][3];
}

function update_lambda_stats(node) {
    var sum_inv = 1 / math.sum(node['lambda_incoming']);
    var lambda_vector = [];
    for (var i = 0; i < node['lambda_incoming'].length; i++) {
        lambda_vector.push(node['lambda_incoming'][i] * sum_inv);
        LAMBDA_INFECTION_MEAN[i] = (LAMBDA_INFECTION_MEAN[i] * LAMBDA_INFECTION_STATS.length + lambda_vector[i]) / (LAMBDA_INFECTION_STATS.length + 1);
    }
    LAMBDA_INFECTION_STATS.push(lambda_vector)
}

let csvContent_alltogether = "data:text/csv;charset=utf-8,Day,Affected,Recovered,Infected,Exposed,Hospitalised,Critical,Fatalities,Lambda_H,Lambda_W,Lambda_C,Daily_New_Exposed,Daily_New_Presymptomatic,Daily_New_Symptomatic,Daily_New_Recovered,Daily_New_Detected,Daily_New_Hospitalised,Daily_New_Critical,Daily_New_Deaths,Cumulative_New_Exposed,Cumulative_New_Presymptomatic,Cumulative_New_Symptomatic,Cumulative_New_Recovered,Cumulative_New_Detected,Cumulative_New_Hospitalised,Cumulative_New_Critical,Cumulative_New_Deaths\r\n";

function update_all_kappa(nodes, homes, workplaces, communities, cur_time) {
    var current_time = cur_time;
    // console.log(NUM_DAYS_BEFORE_INTERVENTIONS);
    if (current_time < NUM_DAYS_BEFORE_INTERVENTIONS * SIM_STEPS_PER_DAY) {
        get_kappa_no_intervention(nodes, homes, workplaces, communities, current_time);
    } else {
        
        switch (INTERVENTION) {
            case CALIBRATION:
                get_kappa_CALIBRATION(nodes, homes, workplaces, communities, current_time);
                break;
            case NO_INTERVENTION:
                get_kappa_no_intervention(nodes, homes, workplaces, communities, current_time);
                break;
            case CASE_ISOLATION:
                get_kappa_case_isolation(nodes, homes, workplaces, communities, current_time);
                break;
            case HOME_QUARANTINE:
                get_kappa_home_quarantine(nodes, homes, workplaces, communities, current_time);
                break;
            case LOCKDOWN:
                get_kappa_lockdown(nodes, homes, workplaces, communities, current_time);
                break;
            case CASE_ISOLATION_AND_HOME_QUARANTINE:
                get_kappa_CI_HQ(nodes, homes, workplaces, communities, current_time);
                break;
            case CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS:
                get_kappa_CI_HQ_70P(nodes, homes, workplaces, communities, current_time);
                break;
            case LOCKDOWN_40_CI_HQ_SD_70_PLUS_21_CI:
                get_kappa_LOCKDOWN_40_CI_HQ_SD_70_PLUS_21_CI(nodes, homes, workplaces, communities, current_time);
                break;
            case LOCKDOWN_40:
                get_kappa_LOCKDOWN_40(nodes, homes, workplaces, communities, current_time);
                break;
            case LD_40_CI_HQ_SD70_SC_21_SC_42:
                get_kappa_LD_40_CI_HQ_SD70_SC_21_SC_42(nodes, homes, workplaces, communities, current_time);
                break;
            case LD_40_CI_HQ_SD70_SC_21:
                get_kappa_LD_40_CI_HQ_SD70_SC_21(nodes, homes, workplaces, communities, current_time);
                break;
            case LD_40_CI_HQ_SD70_SC_OE_30:
                get_kappa_LD_40_CI_HQ_SD70_SC_OE_30(nodes, homes, workplaces, communities, current_time);
                break;
            default:
                break;
        }
    }
}

function run_simday(time_step, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
                    days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered,days_num_symptomatic, lambda_evolution) {
    // console.log(time_step / SIM_STEPS_PER_DAY);
    // Reset daily counters at the beginning of each day
    if (time_step % SIM_STEPS_PER_DAY === 0) {
        reset_daily_counters();
        // console.log("Starting Day " + (Math.floor(time_step / SIM_STEPS_PER_DAY) + 1) + " - Daily counters reset");
    }
    // Seeding strategies
    if (SEEDING_MODE == SEED_INFECTION_RATES && time_step < seed_array.length) {
        infection_seeding(nodes, seed_array[time_step], time_step);
    } else if (SEEDING_MODE == SEED_EXP_RATE) {
        infection_seeding_exp_rate(nodes, time_step);
    }
    // Update infection status for all individuals (this increments daily counters)
    for (var j = 0; j < NUM_PEOPLE; j++) {
        update_infection(nodes[j], time_step);
        update_psi(nodes[j], time_step);
    }
    
    update_all_kappa(nodes, homes, workplaces, communities, time_step);
    
    for (var h = 0; h < NUM_HOMES; h++) {
        homes[h]['age_dependent_mixing'] = update_lambda_h(nodes, homes[h]);
    }
    
    for (var w = 0; w < NUM_SCHOOLS + NUM_WORKPLACES; w++) {
        workplaces[w]['age_dependent_mixing'] = update_lambda_w(nodes, workplaces[w]);
    }
    for (var c = 0; c < NUM_COMMUNITIES; c++) {
        communities[c]['lambda_community'] = update_lambda_c_local(nodes, communities[c]);
        var temp_stats = get_infected_community(nodes, communities[c]);
        var row = [time_step / SIM_STEPS_PER_DAY, c, temp_stats[0], temp_stats[1], temp_stats[2], temp_stats[3], temp_stats[4]].join(",");
        csvContent += row + "\r\n";
    }
    
    for (var pt = 0; pt < NUM_PUBLIC_TRANSPORT; pt++) {
        public_transports[pt]['lambda_PT'] = update_lambda_public_transport(nodes, public_transports[pt]);
    }
    update_lambda_c_global(communities, community_distance_matrix);
    for (var j = 0; j < NUM_PEOPLE; j++) {
        update_lambdas(nodes[j], homes, workplaces, communities, public_transports, nodes, time_step);
    }
    // Calculate current state statistics (EXISTING - keep unchanged)
    var n_infected = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == PRE_SYMPTOMATIC || node['infection_status'] == SYMPTOMATIC || node['infection_status'] == HOSPITALISED || node['infection_status'] == CRITICAL) ? 1 : 0);
    }, 0);
    days_num_infected.push([time_step / SIM_STEPS_PER_DAY, n_infected]);
    var n_symptomatic = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == SYMPTOMATIC) ? 1 : 0);
    }, 0);
    days_num_symptomatic.push([time_step / SIM_STEPS_PER_DAY, n_symptomatic]);
    var n_exposed = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == EXPOSED) ? 1 : 0);
    }, 0);
    days_num_exposed.push([time_step / SIM_STEPS_PER_DAY, n_exposed]);
    var n_hospitalised = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == HOSPITALISED) ? 1 : 0);
    }, 0);
    days_num_hospitalised.push([time_step / SIM_STEPS_PER_DAY, n_hospitalised]);
    var n_critical = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == CRITICAL) ? 1 : 0);
    }, 0);
    days_num_critical.push([time_step / SIM_STEPS_PER_DAY, n_critical]);
    var n_fatalities = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == DEAD) ? 1 : 0);
    }, 0);
    days_num_fatalities.push([time_step / SIM_STEPS_PER_DAY, n_fatalities]);
    var n_recovered = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == RECOVERED) ? 1 : 0);
    }, 0);
    days_num_recovered.push([time_step / SIM_STEPS_PER_DAY, n_recovered]);
    days_num_affected.push([time_step / SIM_STEPS_PER_DAY, NUM_AFFECTED_COUNT]);
    
    // Track daily new symptomatic EVERY timestep
    if (time_step % SIM_STEPS_PER_DAY === (SIM_STEPS_PER_DAY - 1)) {
        var current_day = Math.floor(time_step / SIM_STEPS_PER_DAY) + 1;
        days_daily_new_symptomatic.push([current_day, DAILY_NEW_SYMPTOMATIC]);
    }
    
    // Track daily new detected EVERY timestep
    if (time_step % SIM_STEPS_PER_DAY === (SIM_STEPS_PER_DAY - 1)) {
        var current_day = Math.floor(time_step / SIM_STEPS_PER_DAY) + 1;
        days_daily_new_detected.push([current_day, DAILY_NEW_DETECTED]);
    }

    // Track cumulative affected count EVERY timestep
    days_affected_count.push([time_step / SIM_STEPS_PER_DAY, NUM_AFFECTED_COUNT]);
    // Track daily deaths and recovered EVERY timestep
    // Track daily deaths EVERY timestep
    if (time_step % SIM_STEPS_PER_DAY === (SIM_STEPS_PER_DAY - 1)) {
        var current_day = Math.floor(time_step / SIM_STEPS_PER_DAY) + 1;
        days_daily_new_deaths.push([current_day, DAILY_NEW_DEATHS]);
    }
    
    // Track daily new recovered EVERY timestep
    if (time_step % SIM_STEPS_PER_DAY === (SIM_STEPS_PER_DAY - 1)) {
        var current_day = Math.floor(time_step / SIM_STEPS_PER_DAY) + 1;
        days_daily_new_recovered.push([current_day, DAILY_NEW_RECOVERED]);
    }
    
    // Extended CSV content with all counters
    var row = [
        time_step / SIM_STEPS_PER_DAY, 
        NUM_AFFECTED_COUNT, 
        n_recovered, 
        n_infected,
        n_symptomatic,
        n_exposed, 
        n_hospitalised, 
        n_critical, 
        n_fatalities, 
        LAMBDA_INFECTION_MEAN[0], 
        LAMBDA_INFECTION_MEAN[1], 
        LAMBDA_INFECTION_MEAN[2],
        DAILY_NEW_EXPOSED,
        DAILY_NEW_PRESYMPTOMATIC,
        DAILY_NEW_SYMPTOMATIC,
        DAILY_NEW_RECOVERED,
        DAILY_NEW_DETECTED,
        DAILY_NEW_HOSPITALISED,
        DAILY_NEW_CRITICAL,
        DAILY_NEW_DEATHS,
        CUMULATIVE_NEW_EXPOSED,
        CUMULATIVE_NEW_PRESYMPTOMATIC,
        CUMULATIVE_NEW_SYMPTOMATIC,
        CUMULATIVE_NEW_RECOVERED,
        CUMULATIVE_NEW_DETECTED,
        CUMULATIVE_NEW_HOSPITALISED,
        CUMULATIVE_NEW_CRITICAL,
        CUMULATIVE_NEW_DEATHS
    ].join(",");
    csvContent_alltogether += row + "\r\n";
    
    if (LAMBDA_INFECTION_STATS.length > 0) {
        lambda_evolution.push([time_step / SIM_STEPS_PER_DAY, [LAMBDA_INFECTION_MEAN[0], LAMBDA_INFECTION_MEAN[1], LAMBDA_INFECTION_MEAN[2], LAMBDA_INFECTION_MEAN[3]]])
    }
    // At the end of each day, update cumulative counters AND populate cumulative arrays
    if (time_step % SIM_STEPS_PER_DAY === (SIM_STEPS_PER_DAY - 1)) {
        update_cumulative_counters();
        
        var current_day = Math.floor(time_step / SIM_STEPS_PER_DAY) + 1;
        
        // Track cumulative data for plotting
        // 1. Cumulative infected: total people who have ever been infected ( Exposed )
        days_cumulative_infected.push([current_day, CUMULATIVE_NEW_EXPOSED]);
        
        // 2. Cumulative hospitalised: total people who have ever been hospitalized
        days_cumulative_hospitalised.push([current_day, CUMULATIVE_NEW_HOSPITALISED]);
        
        // 3. Cumulative critical: total people who have ever been in critical care
        days_cumulative_critical.push([current_day, CUMULATIVE_NEW_CRITICAL]);
        
        // 4. Cumulative deaths: total people who have died
        days_cumulative_deaths.push([current_day, CUMULATIVE_NEW_DEATHS]);
        
        // 5. Cumulative recovered: total people who have recovered
        days_cumulative_recovered.push([current_day, CUMULATIVE_NEW_RECOVERED]);

        // 6. Cumulative detected: total detected cases
        days_cumulative_detected.push([current_day, CUMULATIVE_NEW_DETECTED]);
        
        // console.log("Day " + current_day + " Summary:");
        // console.log("  Daily - Exposed: " + DAILY_NEW_EXPOSED + ", Symptomatic: " + DAILY_NEW_SYMPTOMATIC + ", Hospitalised: " + DAILY_NEW_HOSPITALISED + ", Deaths: " + DAILY_NEW_DEATHS);
        // console.log("  Cumulative - Infected Total: " + CUMULATIVE_NEW_EXPOSED + ", Hospitalised: " + CUMULATIVE_NEW_HOSPITALISED + ", Deaths: " + CUMULATIVE_NEW_DEATHS);
    }

    // Also populate cumulative arrays every timestep for smooth plotting
    var last_cumulative_infected, last_cumulative_hospitalised, last_cumulative_critical, last_cumulative_deaths, last_cumulative_recovered, last_cumulative_detected;
    
    if (days_cumulative_infected.length > 0) {
        last_cumulative_infected = days_cumulative_infected[days_cumulative_infected.length - 1][1];
        last_cumulative_hospitalised = days_cumulative_hospitalised[days_cumulative_hospitalised.length - 1][1];
        last_cumulative_critical = days_cumulative_critical[days_cumulative_critical.length - 1][1];
        last_cumulative_deaths = days_cumulative_deaths[days_cumulative_deaths.length - 1][1];
        last_cumulative_recovered = days_cumulative_recovered[days_cumulative_recovered.length - 1][1];
        last_cumulative_detected = days_cumulative_detected[days_cumulative_detected.length - 1][1];
    } else {
        // First timestep - initialize with current cumulative values
        // last_cumulative_infected = CUMULATIVE_NEW_PRESYMPTOMATIC + CUMULATIVE_NEW_SYMPTOMATIC + CUMULATIVE_NEW_HOSPITALISED + CUMULATIVE_NEW_CRITICAL;
        last_cumulative_infected = CUMULATIVE_NEW_EXPOSED;
        last_cumulative_hospitalised = CUMULATIVE_NEW_HOSPITALISED;
        last_cumulative_critical = CUMULATIVE_NEW_CRITICAL;
        last_cumulative_deaths = CUMULATIVE_NEW_DEATHS;
        last_cumulative_recovered = CUMULATIVE_NEW_RECOVERED;
        last_cumulative_detected = CUMULATIVE_NEW_DETECTED;
    }
    // Ensure cumulative arrays have data for every timestep (for smooth plotting)
    if (time_step % SIM_STEPS_PER_DAY !== (SIM_STEPS_PER_DAY - 1)) {
        // Fill in inter-day timesteps with current cumulative values
        days_cumulative_infected.push([time_step / SIM_STEPS_PER_DAY, last_cumulative_infected]);
        days_cumulative_hospitalised.push([time_step / SIM_STEPS_PER_DAY, last_cumulative_hospitalised]);
        days_cumulative_critical.push([time_step / SIM_STEPS_PER_DAY, last_cumulative_critical]);
        days_cumulative_deaths.push([time_step / SIM_STEPS_PER_DAY, last_cumulative_deaths]);
        days_cumulative_recovered.push([time_step / SIM_STEPS_PER_DAY, last_cumulative_recovered]);
        days_cumulative_detected.push([time_step / SIM_STEPS_PER_DAY, last_cumulative_detected]);
    }
    // Continue simulation or return
    if (time_step % SIM_STEPS_PER_DAY == 0 && time_step > 0) {
        document.getElementById("status").innerHTML = "Simulation Complete for " + time_step / SIM_STEPS_PER_DAY + " Days";
        var statusDiv = document.getElementById("status");
        statusDiv.style.visibility = "visible";
        return time_step;
    } else {
        return run_simday(time_step + 1, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
            days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, days_num_symptomatic, lambda_evolution);
    }
}

// Helper functions for counter management
function newFilledArray(len, val) {
    var rv = new Array(len);
    while (--len >= 0) {
        rv[len] = val;
    }
    return rv;
}

function initialize_simulation() {
    document.getElementById("status").innerHTML = "Initializing Simulation...";
    // Reset tracking arrays (use var instead of let/const)
    days_daily_new_symptomatic = [];
    days_cumulative_infected = [];
    days_cumulative_hospitalised = [];
    days_cumulative_critical = [];
    days_cumulative_deaths = [];
    days_cumulative_recovered = [];
    days_affected_count = [];
    days_daily_new_detected = [];
    days_cumulative_detected = [];
    days_daily_new_deaths = [];
    days_daily_new_recovered = [];
    
    
    var homes = init_homes();
    var workplaces = init_workplaces();
    var communities = init_community();
    var public_transports = init_public_transport();
    var nodes = init_nodes();
    var community_distance_matrix = compute_community_distances(communities);
    var seed_array = [];
    if (SEEDING_MODE == SEED_INFECTION_RATES) {
        seed_array = load_infection_seed_json(SEED_SCALING_FACTOR);
    }

    // console.log(NUM_PEOPLE, NUM_HOMES, NUM_WORKPLACES, NUM_SCHOOLS, NUM_COMMUNITIES)
    assign_individual_home_community(nodes, homes, workplaces, communities);
    assign_individual_public_transports(nodes, public_transports, workplaces);

    compute_scale_homes(homes);
    compute_scale_workplaces(workplaces);
    compute_scale_communities(nodes, communities);
    compute_scale_public_transport(nodes, public_transports);

    var array_len = NUM_TIMESTEPS;
    var array_default = 0;
    var days_num_exposed = newFilledArray(array_len, array_default);
    var days_num_infected = newFilledArray(array_len, array_default);
    var days_num_symptomatic = newFilledArray(array_len, array_default);
    var days_num_affected = newFilledArray(array_len, array_default);
    var days_num_hospitalised = newFilledArray(array_len, array_default);
    var days_num_critical = newFilledArray(array_len, array_default);
    var days_num_fatalities = newFilledArray(array_len, array_default);
    var days_num_recovered = newFilledArray(array_len, array_default);

    // Initialize all counters
    reset_daily_counters();
    CUMULATIVE_NEW_EXPOSED = 0;
    CUMULATIVE_NEW_PRESYMPTOMATIC = 0;
    CUMULATIVE_NEW_SYMPTOMATIC = 0;
    CUMULATIVE_NEW_RECOVERED = 0;
    CUMULATIVE_NEW_DETECTED = 0;
    CUMULATIVE_NEW_HOSPITALISED = 0;
    CUMULATIVE_NEW_CRITICAL = 0;
    CUMULATIVE_NEW_DEATHS = 0;
    NUM_DETECTED_CASES = 0;

    var lambda_evolution = []
    LAMBDA_INFECTION_STATS = []; //global variable to track lambda evolution when a person gets infected
    LAMBDA_INFECTION_MEAN = [0, 0, 0, 0];
    NUM_AFFECTED_COUNT = 0;
    
    document.getElementById("status").innerHTML = "Initialization Done...";
    return [homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, days_num_symptomatic, lambda_evolution];
}

SIMULATION_STOP = false;

function stop_sim() {
    SIMULATION_STOP = true;
}

function run_simulation() {
    document.getElementById("run_button").style.display = "none";
    document.getElementById("sim_stop").style.display = "inline";

    // document.getElementById('plots-area').style.display = 'block';
    let [homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, days_num_symptomatic, lambda_evolution] = initialize_simulation();
    document.getElementById("status").innerHTML = "Starting to run Simulation...";
    let time_step = 0;
    time_step = run_simday(time_step, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, days_num_symptomatic, lambda_evolution);

    let plot_tuple = [days_num_infected, days_num_symptomatic, days_num_hospitalised, days_num_critical, days_num_fatalities, days_num_recovered, days_num_affected, days_num_exposed, lambda_evolution];// this is a tuple of all the data to be plotted
    call_plotly(plot_tuple);
    document.getElementById("in-progress").style.display = 'none';
    document.getElementById('plots-area').style.display = 'block';
    
    let cumul_days = 0;
    let interv_ind = 0;

    const interval = setInterval(function () {
        // console.log("inside the interval stuff. time_step = ", time_step);
        document.getElementById("status").innerHTML = "Calculating Simulation for Day: " + time_step / SIM_STEPS_PER_DAY;
	// Days after intervention commencement
        let days = time_step / SIM_STEPS_PER_DAY;

        // Checking if allowed
        if (
            interv_ind < INTERVENTIONS.length &&
            days >= NUM_DAYS_BEFORE_INTERVENTIONS + cumul_days
        ) {
            cumul_days += INTERVENTIONS[interv_ind].time;
            INTERVENTION = INTERVENTIONS[interv_ind].value;
            interv_ind++;
        }
        // console.log({ days, INTERVENTION });
        time_step = run_simday(time_step + 1, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
            days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, days_num_symptomatic, lambda_evolution);
        call_plotly(plot_tuple);
       
        if (time_step >= NUM_TIMESTEPS || SIMULATION_STOP) {
            //console.log("time_step = ", time_step);
            clearInterval(interval);
            //Get Run working again
            document.getElementById("run_button").style.display = "inline";
            document.getElementById("sim_stop").style.display = "none";
            SIMULATION_STOP = false;
            //Do some TASKS for output
            var encodedUri = encodeURI(csvContent);
            var link = document.createElement("a");
            link.setAttribute("href", encodedUri);
            link.setAttribute("download", "my_data.csv");
            document.body.appendChild(link); // Required for FF
            if (!WEBPAGE_VERSION) {
                link.click();	//TODO: Instead of click link, add link for download on page.
            }
            encodedUri = encodeURI(csvContent_alltogether);
            link = document.createElement("a");
            link.setAttribute("href", encodedUri);
            link.setAttribute("download", "my_data_all_together.csv");
            document.body.appendChild(link); // Required for FF
            // NUM_PEOPLE is not always exact size you intended to instantiate, due to stochasticity
            // Below code adjusts the displayed number within +/- 100 for 100K model
            let numKs = NUM_PEOPLE;
            let precBand = 100;
            let residue = NUM_PEOPLE%(precBand*10)
            if (residue <= precBand) { 
                numKs = NUM_PEOPLE - residue; 
            }
            else if (residue >= precBand*9) {
                numKs = NUM_PEOPLE + ((precBand*10)-residue);
            }

            document.getElementById("status").innerHTML = "Numbers plotted are per " + String(numKs) + ".";
            if (!WEBPAGE_VERSION) {
                link.click();	//TODO: Instead of click link, add link for download on page.
            }
        }
    }, 150);

    if (LAMBDA_INFECTION_STATS.length > 0) {
        // console.log(math.mean(LAMBDA_INFECTION_STATS, 0));
    }

}

// Enhanced plot_lambda_evolution function with proper sizing
function plot_lambda_evolution(data, plot_position, title_text, legends) {
    if (data[0] == undefined || data[0][0] == undefined) {
        //If there is no data to plot, return.
        return;
    }
    var trace = [];

    for (var count = 0; count < data.length; count++) {
        for (var lambda_length_count = 0; lambda_length_count < data[0][0][1].length; lambda_length_count++) {
            //iterate over lambda_h, lambda_w,lambda_c
            var trace1 = {
                x: [],
                y: [],
                mode: 'bar',
                name: legends[lambda_length_count]
            };
            for (var count2 = 0; count2 < data[count].length; count2++) {
                trace1.x.push(data[count][count2][0]);
                trace1.y.push(data[count][count2][1][lambda_length_count]);
            }
            trace.push(trace1)
        }
    }
    var data_plot = trace;
    // Get container dimensions for responsive sizing
    var container = document.getElementById(plot_position);
    var containerWidth = container ? container.offsetWidth : 400;
    var containerHeight = 320; // Match other plots
    
    // Calculate responsive dimensions
    var plotWidth = Math.max(300, containerWidth - 20);
    var plotHeight = containerHeight;
    var layout = {
        autosize: false,
        width: plotWidth,
        height: plotHeight,
        barmode: 'stack',
        margin: { 
            l: 50,    // Reduced margins to match other plots
            r: 30, 
            t: 40, 
            b: 40 
        },
        title: {
            text: title_text,
            font: {
                family: 'Arial, sans-serif',
                size: 14  // Smaller font to match other plots
            },
            xref: 'paper',
            x: 0.5,
        },
        xaxis: {
            fixedrange: getFixedRange(),
            title: {
                text: 'Days',
                font: {
                    family: 'Arial, sans-serif',
                    size: 12,  // Smaller font
                    color: '#333333'
                }
            },
            tickfont: { size: 10 }  // Smaller tick labels
        },
        yaxis: {
            fixedrange: getFixedRange(),
            title: {
                text: 'Fraction of Transmission',
                font: {
                    family: 'Arial, sans-serif',
                    size: 12,  // Smaller font
                    color: '#333333'
                }
            },
            range: [0, 1],
            tickfont: { size: 10 }  // Smaller tick labels
        },
        showlegend: true,
        legend: {
            x: 0.98,
            xanchor: 'right',
            y: 0.98,
            yanchor: 'top',
            bgcolor: 'rgba(255, 255, 255, 0.9)',
            bordercolor: '#cccccc', 
            borderwidth: 1,
            font: { size: 9, color: '#333333' }, // Smaller legend font
            itemsizing: 'constant', 
            itemwidth: 25
        },
        plot_bgcolor: '#ffffff',
        paper_bgcolor: '#ffffff'
    };

    // Plotly configuration to match other plots
    var config = {
        modeBarButtonsToRemove: [
            'hoverClosestCartesian', 
            'hoverCompareCartesian',
            'lasso2d',
            'select2d',
            'zoom2d',
            'pan2d',
            'zoomIn2d',
            'zoomOut2d',
            'autoScale2d'
        ],
        displayModeBar: false, // Hide modebar for cleaner look
        displaylogo: false,
        responsive: true
    };
    try {
        Plotly.newPlot(plot_position, data_plot, layout, config);
        
        // console.log('Successfully plotted lambda evolution with proper sizing');
    } catch (error) {
        console.error('Error plotting lambda evolution:', error);
        // Fallback for older Plotly versions
        Plotly.newPlot(plot_position, data_plot, layout);
    }
}

function getFixedRange() {
    if($('body').hasClass('mobile')) {
        return true;
    }
    return false;
}

// ADD THIS helper function: for reporting cases based on infection data
function calculateReportedCases(infectiousData) {
    var reportedData = [];
    for (var i = 0; i < infectiousData.length; i++) {
        if (Array.isArray(infectiousData[i]) && infectiousData[i].length >= 2) {
            var day = infectiousData[i][0];
            var reportedCount = Math.round(infectiousData[i][1] * CASE_INFECTION_RATIO);
            reportedData.push([day, reportedCount]);
        }
    }
    return reportedData;
}

function call_plotly(data_tuple) {
    originalPlotData = {};
    
    // Store original data for toggle functionality
    originalPlotData['num_infected_plot'] = {
        dailyData: [data_tuple[0]], 
        cumulativeData: [days_cumulative_infected], 
        title: 'Infectious',
        legends: ['Infected Population']
    };
    
    originalPlotData['num_cases_plot'] = {
        dailyData: [data_tuple[1]],
        cumulativeData: [days_affected_count], 
        title: 'Symptomatic',
        legends: ['Cases']
    };

    originalPlotData['num_reported_plot'] = {
    dailyData: [days_daily_new_detected],
    cumulativeData: [days_cumulative_detected],
    title: 'Reported Cases',
    legends: ['Reported Cases']
    };

    originalPlotData['num_hospitalised_plot'] = {
        dailyData: [data_tuple[2]], 
        cumulativeData: [days_cumulative_hospitalised], 
        title: 'Hospitalised',
        legends: ['Hospitalised Population']
    };
    
    originalPlotData['num_critical_plot'] = {
        dailyData: [data_tuple[3]], 
        cumulativeData: [days_cumulative_critical], 
        title: 'Critical',
        legends: ['Critical Population']
    };
    
    originalPlotData['num_fatalities_plot'] = {
        dailyData: [days_daily_new_deaths],     
        cumulativeData: [days_cumulative_deaths], 
        title: 'Fatalities',
        legends: ['Fatalities']
    };
    
    originalPlotData['num_recovered_plot'] = {
        dailyData: [days_daily_new_recovered], 
        cumulativeData: [days_cumulative_recovered], 
        title: 'Recovered',
        legends: ['Recovered Population']
    };
    
    // Plot all charts with daily data initially - use traditional for...in loop
    var plotIds = Object.keys(originalPlotData);
    for (var i = 0; i < plotIds.length; i++) {
        var plotId = plotIds[i];
        if (originalPlotData[plotId].dailyData && originalPlotData[plotId].dailyData[0]) {
            plot_plotly(
                originalPlotData[plotId].dailyData,
                plotId,
                originalPlotData[plotId].title + ' (daily)',
                originalPlotData[plotId].legends,
                false
            );
        }
    }
    
    // Plot source of infection (unchanged)
    if (data_tuple[8]) {
        plot_lambda_evolution(
            [data_tuple[8]], 
            'lambda_evolution', 
            'Source of infection', 
            ['Home', 'School/Workplace', 'Community', 'Public Transport']
        );
    }
    
    setupToggleButtons();

    preserveCustomInterventionState();
}

// Make sure to call this when the DOM is ready
document.addEventListener('DOMContentLoaded', function() {
    setupToggleButtons();
});

function setupToggleEventListeners(charts) {
    // Set up event listeners for all charts
    charts.forEach(chart => {
        const dailyButtonId = `daily-${chart.id.replace('num_', '').replace('_plot', '')}`;
        const cumulativeButtonId = `cumulative-${chart.id.replace('num_', '').replace('_plot', '')}`;
        
        document.getElementById(dailyButtonId)?.addEventListener('change', function() {
            if (this.checked) {
                plot_plotly(chart.data, chart.id, chart.title, [chart.legend], false);
            }
        });
        
        document.getElementById(cumulativeButtonId)?.addEventListener('change', function() {
            if (this.checked) {
                plot_plotly(chart.data, chart.id, chart.title.replace('daily', 'cumulative'), 
                           [chart.legend], true);
            }
        });
    });
}

function run_and_plot(intervention) {
    var returned_values;
    returned_values = run_simulation();
    call_plotly(returned_values);
}

function plotly_extend(div_id, x_value, y_value) {
    Plotly.extendTraces(div_id, {
        x: [[x_value]],
        y: [[y_value]]
    }, [0]);
}

const plot_minRanges = {
    'num_infected_plot': 50,
    'num_cases_plot': 100,
    'num_hospitalised_plot': 20,
    'num_critical_plot': 10,
    'num_fatalities_plot': 5,
    'num_recovered_plot': 100
};

// In your plot_plotly function, make sure traces are created as line graphs:
function plot_plotly(data, plotId, titleText, legends, isCumulative) {
    if (isCumulative === undefined) {
        isCumulative = false;
    }
    // console.log("Plotting", plotId, "with isCumulative =", isCumulative);
    var displayData = data;
    // Create traces with explicit line mode
    var traces = [];
    var colorPalette = [
        '#1f77b4', // Blue
        '#ff7f0e', // Orange  
        '#2ca02c', // Green
        '#d62728', // Red
        '#9467bd', // Purple
        '#8c564b'  // Brown
    ];
    
    for (var i = 0; i < displayData.length; i++) {
        var trace = {
            x: [],
            y: [],
            mode: 'lines+markers',
            name: legends[i] || 'Data ' + (i + 1),
            type: 'scatter',
            line: { 
                width: 2, // Slightly thinner for smaller plots
                color: colorPalette[i % colorPalette.length],
                shape: 'linear'
            },
            marker: {
                size: 3, // Smaller markers for compact plots
                color: colorPalette[i % colorPalette.length]
            }
        };
        // Populate data points
        for (var j = 0; j < displayData[i].length; j++) {
            if (Array.isArray(displayData[i][j]) && displayData[i][j].length >= 2) {
                trace.x.push(displayData[i][j][0]);
                trace.y.push(displayData[i][j][1]);
            }
        }
        
        traces.push(trace);
    }
    // Get container dimensions for responsive sizing
    var container = document.getElementById(plotId);
    var containerWidth = container ? container.offsetWidth : 400;
    var containerHeight = 320; // Fixed height for consistency
    
    // Calculate responsive dimensions
    var plotWidth = Math.max(300, containerWidth - 20); // Minimum 300px, with 20px padding
    var plotHeight = containerHeight;
    
    // Enhanced layout for better fit in smaller containers
    var yValues = traces.length > 0 ? traces[0].y.filter(function(y) { return !isNaN(y) && isFinite(y); }) : [];
    var yMax = yValues.length > 0 ? Math.max.apply(Math, yValues) : 100;
    
    var layout = {
        autosize: false,
        width: plotWidth,
        height: plotHeight,
        margin: { 
            l: 50,    // Reduced left margin
            r: 30,    // Reduced right margin
            t: 40,    // Reduced top margin
            b: 40     // Reduced bottom margin
        },
        title: {
            text: titleText,
            font: { family: 'Arial, sans-serif', size: 14, color: '#333333' }, // Smaller title
            xref: 'paper',
            x: 0.5,
            y: 0.95
        },
        xaxis: {
            title: {
                text: 'Days',
                font: { family: 'Arial, sans-serif', size: 12, color: '#333333' } // Smaller axis labels
            },
            range: [0, NUM_DAYS],
            autorange: false,
            gridcolor: '#e6e6e6',
            linecolor: '#666666',
            zeroline: false,
            showgrid: true,
            tickfont: { size: 10 } // Smaller tick labels
        },
        yaxis: {
            title: {
                text: isCumulative ? 'Cumulative Count' : 'Daily Count',
                font: { family: 'Arial, sans-serif', size: 12, color: '#333333' }
            },
            gridcolor: '#e6e6e6',
            linecolor: '#666666',
            zeroline: true,
            zerolinecolor: '#cccccc',
            showgrid: true,
            tickfont: { size: 10 },
            rangemode: 'tozero' // Ensure y-axis starts at zero
        },
        plot_bgcolor: '#ffffff',
        paper_bgcolor: '#ffffff',
        showlegend: false,
        legend: {
            x: 0.98, 
            y: 0.98, 
            xanchor: 'right', 
            yanchor: 'top',
            bgcolor: 'rgba(255, 255, 255, 0.9)',
            bordercolor: '#cccccc', 
            borderwidth: 1,
            orientation: 'v', 
            font: { size: 9, color: '#333333' }, // Smaller legend font
            itemsizing: 'constant', 
            itemwidth: 25, // Smaller legend items
            traceorder: 'normal'
        },
        hovermode: 'x unified'
    };
    
    // Set y-axis range - FIXED VERSION
    var plotMinRange = plot_minRanges[plotId] || 100;
    if (yMax * 1.3 <= plotMinRange) {
        layout.yaxis.range = [0, plotMinRange];
        layout.yaxis.autorange = false;
    } else {
        // Instead of autorange, set a manual range starting from 0
        layout.yaxis.range = [0, Math.max(yMax * 1.1, plotMinRange)];
        layout.yaxis.autorange = false;
    }

// Ensure y-axis always starts from 0
layout.yaxis.rangemode = 'tozero';
    
    // Plotly configuration for better performance in smaller containers
    var config = {
        modeBarButtonsToRemove: [
            'hoverClosestCartesian', 
            'hoverCompareCartesian',
            'lasso2d',
            'select2d',
            'zoom2d',
            'pan2d',
            'zoomIn2d',
            'zoomOut2d',
            'autoScale2d'
        ],
        displayModeBar: false, // Hide modebar for cleaner look in small plots
        displaylogo: false,
        responsive: true,
        staticPlot: false
    };
    
    try {
        Plotly.newPlot(plotId, traces, layout, config);
        
        // console.log('Successfully plotted ' + plotId + ' as responsive line graph');
    } catch (error) {
        console.error('Error plotting ' + plotId + ':', error);
        // Fallback for older Plotly versions
        Plotly.newPlot(plotId, traces, layout);
    }
}

// Update the handlePlotResize function to include lambda plot
function handlePlotResize() {
    var plotIds = [
        "num_infected_plot",
        "num_cases_plot", 
        "num_reported_plot",
        "num_hospitalised_plot",
        "num_critical_plot", 
        "num_fatalities_plot",
        "num_recovered_plot",
        "lambda_evolution"  // Make sure this is included
    ];
    
    plotIds.forEach(function(plotId) {
        var plotDiv = document.getElementById(plotId);
        if (plotDiv && plotDiv.layout) {
            var newWidth = Math.max(300, plotDiv.offsetWidth - 20);
            Plotly.relayout(plotId, {
                width: newWidth,
                height: 320
            });
        }
    });
}

// Add resize event listener
window.addEventListener('resize', function() {
    if (window.resizeTimeout) {
        window.clearTimeout(window.resizeTimeout);
    }
    window.resizeTimeout = window.setTimeout(handlePlotResize, 250);
});

function setupToggleButtons() {
    // Daily toggle buttons - use traditional DOM methods like sim.js
    var dailyButtons = document.querySelectorAll('[id^="daily-"]');
    for (var i = 0; i < dailyButtons.length; i++) {
        dailyButtons[i].addEventListener('change', function() {
            if (this.checked) {
                var chartType = this.id.replace('daily-', '');
                var plotId = 'num_' + chartType + '_plot';
                
                if (originalPlotData[plotId]) {
                    plot_plotly(
                        originalPlotData[plotId].dailyData,
                        plotId,
                        originalPlotData[plotId].title + ' (daily)',
                        originalPlotData[plotId].legends,
                        false
                    );
                }
                preserveCustomInterventionState();
            }
        });
    }
    
    // Cumulative toggle buttons
    var cumulativeButtons = document.querySelectorAll('[id^="cumulative-"]');
    for (var i = 0; i < cumulativeButtons.length; i++) {
        cumulativeButtons[i].addEventListener('change', function() {
            if (this.checked) {
                var chartType = this.id.replace('cumulative-', '');
                var plotId = 'num_' + chartType + '_plot';
                
                if (originalPlotData[plotId]) {
                    plot_plotly(
                        originalPlotData[plotId].cumulativeData,
                        plotId,
                        originalPlotData[plotId].title + ' (cumulative)',
                        originalPlotData[plotId].legends,
                        true
                    );
                }
                preserveCustomInterventionState();
            }
        });
    }
}

function preserveCustomInterventionState() {
    // Check if custom intervention (value 11) is selected
    var customRadio = document.querySelector('input[name="interventions2"][value="11"]');
    if (customRadio && customRadio.checked) {
        // Ensure custom intervention section stays visible
        var customIntvElement = document.getElementById("customIntv");
        if (customIntvElement) {
            customIntvElement.style.display = "block";
        }
    }
}

//Main function
function runSimulations() {
    //clear_plots();

    //get the inputs from the HTML page
    NUM_DAYS = document.getElementById("numDays").value; // == 0 ? NUM_DAYS : document.getElementById("numDays").value;
    NUM_TIMESTEPS = NUM_DAYS * SIM_STEPS_PER_DAY;

    // initFrac is actually number Exposed, now input as percentage instead of fraction
    INIT_FRAC_INFECTED = parseFloat(document.getElementById("initFrac").value) / 100.0;

    INCUBATION_PERIOD = parseFloat(document.getElementById("Incubation").value) / 2;
    INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD * SIM_STEPS_PER_DAY; // 2.29 days
    CASE_INFECTION_RATIO = parseFloat(document.getElementById("caseInfectionRatio").value);

    MEAN_ASYMPTOMATIC_PERIOD = document.getElementById("asymptomaticMean").value;
    MEAN_SYMPTOMATIC_PERIOD = document.getElementById("symptomaticMean").value;
    SYMPTOMATIC_FRACTION = document.getElementById("symtomaticFraction").value;
    MEAN_HOSPITAL_REGULAR_PERIOD = document.getElementById("meanHospitalPeriod").value;
    MEAN_HOSPITAL_CRITICAL_PERIOD = document.getElementById("meanICUPeriod").value;
    COMPLIANCE_PROBABILITY = document.getElementById("compliance").value;

    ASYMPTOMATIC_PERIOD = MEAN_ASYMPTOMATIC_PERIOD * SIM_STEPS_PER_DAY;
    SYMPTOMATIC_PERIOD = MEAN_SYMPTOMATIC_PERIOD * SIM_STEPS_PER_DAY;
    HOSPITAL_REGULAR_PERIOD = MEAN_HOSPITAL_REGULAR_PERIOD * SIM_STEPS_PER_DAY;
    HOSPITAL_CRITICAL_PERIOD = MEAN_HOSPITAL_CRITICAL_PERIOD * SIM_STEPS_PER_DAY;

    BETA_H = document.getElementById("betaHouse").value;
    BETA_W = document.getElementById("betaWork").value;
    BETA_C = document.getElementById("betaCommunity").value;
    BETA_S = document.getElementById("betaSchools").value;
    BETA_PT = document.getElementById("betaPT").value;

    INTERVENTIONS = [];
    INTERVENTION = NO_INTERVENTION;
    //INTERVENTION = parseInt(document.getElementById("interventions").value);
    if (parseInt(document.querySelector('input[name="interventions2"]:checked').value) == 11){
        li_interventions = document.getElementsByClassName("interv-li");
        NUM_DAYS_BEFORE_INTERVENTIONS = 0;

        for (let i = 0; i < li_interventions.length; i++) {
            div = li_interventions[i].children[0];
            value = div.children[0].value;
            // console.log(2);
            time = div.children[1].value;
            // console.log(3);
            INTERVENTIONS.push({
                value: parseInt(value),
                time: parseInt(time),
            });
        }
        //Set the no intervention after all custom interventions are done
        INTERVENTIONS.push({
            value: 0,
            time: 1,
        });
    }
    else{
        INTERVENTION = parseInt(document.querySelector('input[name="interventions2"]:checked').value);
    }
  
    // console.log(NUM_DAYS, INIT_FRAC_INFECTED, INTERVENTION);
    // console.log("INTERVENTION = ", INTERVENTION);
    //where simulation starts
    run_simulation();
}

function clear_plots() {
    // Test and set visibility
    var plotsAreaEl = document.getElementById('plots-area');
    if (plotsAreaEl) {
        plotsAreaEl.style.display = 'none';
    }
    
    var noDataEl = document.getElementById("no-data");
    if (noDataEl) noDataEl.style.display = 'none';
    
    var inProgressEl = document.getElementById("in-progress");
    if (inProgressEl) inProgressEl.style.display = 'block';
    
    var statusEl = document.getElementById("status");
    if (statusEl) {
        statusEl.innerHTML = "Simulation in Progress....";
        statusEl.style.display = 'inline';
    }
    
    // Clear plot divs with Plotly purge
    var plotIds = [
        "num_infected_plot",
        "num_cases_plot", 
        "num_reported_plot",
        "num_hospitalised_plot",
        "num_critical_plot", 
        "num_fatalities_plot",
        "num_recovered_plot",
        "lambda_evolution"
    ];
    
    for (var i = 0; i < plotIds.length; i++) {
        var plotEl = document.getElementById(plotIds[i]);
        if (plotEl) {
            // Properly destroy Plotly instance
            if (plotEl.data) {
                Plotly.purge(plotEl);
            }
            plotEl.innerHTML = "";
        }
    }
    
    // **MEMORY CLEANUP - Call before starting new simulation**
    clearSimulationMemory();
    
    // Reset toggle buttons
    var toggles = document.querySelectorAll('input[id^="daily-"]');
    for (var i = 0; i < toggles.length; i++) {
        toggles[i].checked = true;
    }
    
    if($('body').hasClass('mobile')) {
        $('.tabs li#right-btn').trigger('click');
    }

    window.setTimeout(function() {
        runSimulations();
    }, 1);
}

function set_default_values_html() {
    document.getElementById("numDays").value = NUM_DAYS;
    document.getElementById("initFrac").value = 100 * INIT_FRAC_INFECTED;
    document.getElementById("Incubation").value = 2 * INCUBATION_PERIOD;
    document.getElementById("asymptomaticMean").value = MEAN_ASYMPTOMATIC_PERIOD;
    document.getElementById("symptomaticMean").value = MEAN_SYMPTOMATIC_PERIOD;
    document.getElementById("symtomaticFraction").value = SYMPTOMATIC_FRACTION;
    document.getElementById("meanHospitalPeriod").value = MEAN_HOSPITAL_REGULAR_PERIOD;
    document.getElementById("meanICUPeriod").value = MEAN_HOSPITAL_CRITICAL_PERIOD;
    //document.getElementById("interventions").value = "0";
    setCity( document.getElementById("cityname").value );
}

function setParameters(city){
    $("#customIntv").hide();
    if (city === 'bengaluru'){
        $("input[name=interventions2][value='0']").prop("checked",true);
        document.getElementById("compliance").value = 0.9;
        document.getElementById("betaHouse").value = 0.9632;
        document.getElementById("betaWork").value = 0.5518;
        document.getElementById("betaCommunity").value = 0.2035;
        document.getElementById("betaSchools").value = 1.1036;
        document.getElementById("betaPT").value = 0;
        CALIBRATION_DELAY = 0; //// Assuming the simulator starts on March 1.
        NUM_DAYS_BEFORE_INTERVENTIONS = 15 + CALIBRATION_DELAY;
    }
    if (city === 'wuhan'){
        document.getElementById("compliance").value = 0.9;
        document.getElementById("betaHouse").value = 1;
        document.getElementById("betaWork").value = 0.65;
        document.getElementById("betaCommunity").value = 0.353;
        document.getElementById("betaSchools").value = 1.3;
        document.getElementById("betaPT").value = 0;
        CALIBRATION_DELAY = 0; //// Assuming the simulator starts on March 1.
        NUM_DAYS_BEFORE_INTERVENTIONS = 22 + CALIBRATION_DELAY;
        $("input[name=interventions2][value='11']").prop("checked",true);
        $("#customIntv").show();
    }
    if (city === 'nyc'){
        document.getElementById("compliance").value = 0.9;
        document.getElementById("betaHouse").value = 1.902;
        document.getElementById("betaWork").value = 1.583;
        document.getElementById("betaCommunity").value = 0.625;
        document.getElementById("betaSchools").value = 3.167;
        document.getElementById("betaPT").value = 0;
        CALIBRATION_DELAY = 0; //// Assuming the simulator starts on March 1.
        NUM_DAYS_BEFORE_INTERVENTIONS = 8 + CALIBRATION_DELAY;
        $("input[name=interventions2][value='11']").prop("checked",true);
        $("#customIntv").show();
    }
    if (city === 'kochi'){
        $("input[name=interventions2][value='0']").prop("checked",true);
        document.getElementById("compliance").value = 0.9;
        document.getElementById("betaHouse").value = 1.065;
        document.getElementById("betaWork").value = 0.532;
        document.getElementById("betaCommunity").value = 0.207;
        document.getElementById("betaSchools").value = 1.064;
        document.getElementById("betaPT").value = 0;
        CALIBRATION_DELAY = 0; //// Assuming the simulator starts on March 1.
        NUM_DAYS_BEFORE_INTERVENTIONS = 15 + CALIBRATION_DELAY;
    }
    if (city === 'mumbai'){
        $("input[name=interventions2][value='0']").prop("checked",true);
        document.getElementById("compliance").value = 0.9;
        document.getElementById("betaHouse").value = 0.911;
        document.getElementById("betaWork").value = 0.488;
        document.getElementById("betaCommunity").value = 0.22;
        document.getElementById("betaSchools").value = 0.976;
        document.getElementById("betaPT").value = 0;
        CALIBRATION_DELAY = 0; //// Assuming the simulator starts on March 1.
        NUM_DAYS_BEFORE_INTERVENTIONS = 15 + CALIBRATION_DELAY;
    }
}

function setCity (city) {
    cityName = city;
    inputPath = INPUTPATHPREFIX + '/' + cityName + '/';
    mapImage.src = IMAGEPREFIX + '/' + cityName + '-wards_v2.png'
    setParameters(city);
}

function toggleTooltip(element) {
    // Hide all other tooltips first
    var allTooltips = document.querySelectorAll('.info-tooltip');
    for (var i = 0; i < allTooltips.length; i++) {
        if (allTooltips[i] !== element.querySelector('.info-tooltip')) {
            allTooltips[i].style.display = 'none';
        }
    }
    
    // Toggle the clicked tooltip
    var tooltip = element.querySelector('.info-tooltip');
    if (tooltip) {
        if (tooltip.style.display === 'none' || tooltip.style.display === '') {
            tooltip.style.display = 'block';
        } else {
            tooltip.style.display = 'none';
        }
    }
}

// Add this function to handle outside clicks
function setupTooltipHandlers() {
    // Hide tooltips when clicking outside
    document.addEventListener('click', function(event) {
        if (!event.target.closest('.info-icon')) {
            var allTooltips = document.querySelectorAll('.info-tooltip');
            for (var i = 0; i < allTooltips.length; i++) {
                allTooltips[i].style.display = 'none';
            }
        }
    });
}


//jquery events for the webUI
$(document).ready(function () {
    if (window.matchMedia("(max-width: 767px)").matches)  {
    fetch("simulator/html/body_mobile.html")
  .then(response => {
    return response.text()
  })
  .then(data => {
    $("body").html(data).addClass('mobile');
  }).then(()=>{
    let h = $(window).outerHeight() - ($('.custom-header').outerHeight() + $('.sticky-footer').outerHeight());
    $('.content-mobile, .left-pane, .right-pane').css('height',h - 25 );
    set_default_values_html();
    initListner();
  });
}
else {
    fetch("simulator/html/body_desktop.html")
  .then(response => {
    return response.text()
  })
  .then(data => {
    $("body").html(data);
  }).then(()=>{
    let w = $(window).width() / 3
    $("#MySplitter").splitter({resizeToWidth: true, sizeLeft:w});
    set_default_values_html();
    initListner();
  });
}
});


function initListner() {
    
    $('#numDays').tooltip({'trigger':'focus', 'title': 'Number of days to run the simulation'});
    $('#Incubation').tooltip({'trigger':'focus', 'title': 'Once exposed to the virus, the mean duration before the exposed individual starts transmitting the virus.'});

    $('#asymptomaticMean').tooltip({'trigger':'focus', 'title': 'The mean duration for which asymptomatic or presymptomatic individuals may transmit the virus before recovering or before symptoms begin to show.'});

    $('#symptomaticMean').tooltip({'trigger':'focus', 'title': 'If symptoms begin to show, the mean duration for which an infected individual exhibits symptoms and continues to transmit the virus.'});

    $('#symtomaticFraction').tooltip({'trigger':'focus', 'title': 'The fraction of the exposed individuals that show symptoms.'});

    $('#meanHospitalPeriod').tooltip({'trigger':'focus', 'title': 'Some symptomatic individuals may need hospital care. If admitted, the mean duration of their stay in a regular ward.'});

    $('#meanICUPeriod').tooltip({'trigger':'focus', 'title': 'Some hospitalised individuals may develop complications, and may need intensive care. If so, the mean duration of their stay in an intensive care unit.'});

    $('#betaHouse').tooltip({'trigger':'focus', 'title': 'The expected number of eventful (infection spreading) contact opportunities in a day at home.'});

    $('#betaWork').tooltip({'trigger':'focus', 'title': 'The expected number of eventful contact opportunities in a day at the workplace.'});

    $('#betaSchools').tooltip({'trigger':'focus', 'title': 'The expected number of eventful contact opportunities in a day at the school.'});

    $('#betaCommunity').tooltip({'trigger':'focus', 'title': 'The expected number of eventful contact opportunities in the community interaction space.'});

    $('#betaPT').tooltip({'trigger':'focus', 'title': 'The expected number of eventful contact opportunities in the transport interaction space.'});

    $('#initFrac').tooltip({'trigger':'focus', 'title': 'What percentage of population is exposed to the virus? The residual incubation period is random.'});

    $('#compliance').tooltip({'trigger':'focus', 'title': 'What fraction of households are likely to follow the restrictions specified in the chosen intervention?'});

    $('#li-interv-time').tooltip({'trigger':'focus', 'title': 'How long does the chosen intervention last?'});
    
    $('#li-interv-select').tooltip({'trigger':'focus', 'title': 'What intervention would you want to select?'});
    
    // Add tooltip for case infection ratio
    $('#caseInfectionRatio').tooltip({
        'trigger': 'focus', 
        'title': 'The proportion of actual infections that are detected and reported as cases. A value of 0.2 means only 20% of infections are detected. This affects the reported case count shown as a dashed line on the infections plot.'
    });

    // City selector listener
    $('.selectpicker').on('change', function () {
        var selectedItem = $('.selectpicker').val();
        setCity(selectedItem);
    });

    // Toggle info for various sections
    $('.toggleInfo').on('click',function() {
        let att = $(this).attr('data-field');
        let htmlInner;
        switch(att) {
            case 'DiseaseProgression':
                htmlInner =  '<img width="100%" src="'+ IMAGEPREFIX +'/InfectionProgression-v2.png" />'
                break;
            case 'Intervention':
                htmlInner = `<span>Simulations under all scenarios are assumed to begin on March 1, 2020. Interventions begin on March 25, aligned with the India lockdown start date.  They continue for the indicated duration (or indefinitely if not specified)
                    <br></br>
                <b>Case Isolation</b>: One day after onset of symptoms, the individual is assumed to stay isolated at home for a period of seven days.
                <br></br>
                <b>Home Quarantine</b>: One day after onset of symptoms, all members of the individual's household stay at home for fourteen days.
                <br></br>
                <b>Lockdown</b>: All schools and colleges are closed. Only essential services are operational. Community interactions are also reduced.
                </span>`
                break;
            case 'SurveillanceParams':
                htmlInner = '<p><strong>Case-to-Infection Ratio (ρ)</strong>: The fraction of all infections detected by surveillance.</p>' +
                    '<p><strong>Mathematical Definition</strong>: If C(t) = reported cases and I(t) = actual infections at time t, then:</p>' +
                    '<p style="text-align: center;">C(t) = ρ × I(t)</p>' +
                    '<p>A low ratio (e.g., ρ = 0.2) means many infections remain undetected (80% in this example).</p>' +
                    '<p>This parameter modifies the simulation to show both:</p>' +
                    '<ul style="padding-left: 15px;">' +
                    '<li>Actual infections (solid line)</li>' +
                    '<li>Reported cases (dashed line)</li>' +
                    '</ul>' +
                    '<p>Understanding this gap is crucial for epidemic management, as real-world surveillance systems never detect all infections.</p>';
                break;
        }
        if(($(this).next()).hasClass('tool-dec')) {
            $(this).next().remove();
        }
        else {
            $(this).after(`<div class="tool-dec">${htmlInner}</div>`);
        }
    })

    // About section collapse handlers
    $('#about').on('shown.bs.collapse', function () {
        $('.abt-toggle .down').hide();
        $('.abt-toggle .up').show();
    })
    $('#about').on('hidden.bs.collapse', function () {
        $('.abt-toggle .up').hide();
        $('.abt-toggle .down').show();
    })

    // Tab navigation
    $('.tabs li').on('click',function() {
        let attr = $(this).attr('data-tab');
        $('.tabs .active').removeClass('active');
        $(this).addClass('active');
        if(attr === 'left') {
            $('#right').hide();
            $('#left').show();
        }
        else {
            $('#left').hide();
            $('#right').show();
        }
    })

    // Window resize handler
    if(!$('body').hasClass('mobile')) {
        $(window).resize();
    }

    // Custom intervention setup
    $("#customIntv").hide();
    $('input[type="radio"]').on('click, change, input', function(){
        if ($(this).is(':checked'))
        {
            var value = $(this).val();
            if (value == 11){
                var msg = "Click the 'Add Intervention' button to set-up the simulator to run a custom intervention.<br> The simulations with custom interventions, start with 100 infected individuals by default on Day 0. After the last intervention we switch to No Intervention scenario.";
                $("#interv").html(msg);
                $("#customIntv").show();
                // Create a new list item when clicking on the "Add" button
                $("#intvButton").on('click', function(){
                    console.log("custom intervention added")
                    newInterv()
                });
            }
            else{
                $("#customIntv").hide();
            }
        }
    });
    setupTooltipHandlers();
}

// Window resize function (outside initListner)
$(window).resize(function() {
    if(!$('body').hasClass('mobile')) {
        let h = $(window).outerHeight() - ($('.custom-header').outerHeight() + $('.footer').outerHeight());
        $('.splitter-container').css('height',h);

        let h1 = $('.splitter-container').outerHeight() - $('.run-btn-cnt').outerHeight();
        $('.scroll').css('height',h1);

        let h2 = $('.splitter-container').outerHeight() - $('.right-top').outerHeight();
        $('.sim-lane').css('height',h2);
    }
})

// Create new intervention function
function newInterv() {
    let count = 0

    const mapping =  {
        "0": "No Intervention (NI)",
        "1": "Case Isolation (CI)",
        "2": "Home Quarantine (HQ)",
        "3": "Lockdown (LD)",
        "4": "CI and HQ of infected household (CIHQ)",
        "5": "CIHQ and social distancing of those above 65 years of age"
    };

    let li = document.createElement("li");
    li.number = count
    li.className = "interv-li"

    // Create li div for everything
    let liDiv = document.createElement("div");
    liDiv.className = "li-interv-div"
    let selectInterv = document.createElement("select")
    const createOption = (value, text) => {
        let option = document.createElement("option");
        option.appendChild(document.createTextNode(text))
        option.value = String(value)
        return option;
    }

    for (let code in mapping) {
        selectInterv.appendChild(createOption(code, mapping[code]))
    }
    selectInterv.className = "li-interv-select"

    let input = document.createElement("input")
    input.className = "li-interv-time"
    input.type = "text"
    input.placeholder = "Duration in days"

    liDiv.appendChild(selectInterv)
    liDiv.appendChild(input)

    li.appendChild(liDiv);

    let close = document.createElement("input");
    close.className = "li-close"
    close.type = "button"
    close.value = "\u2715"
    liDiv.appendChild(close);
    close.onclick = () => {
        li.parentNode.removeChild(li);
    }

    document.getElementById("interv").appendChild(li);

    count++;
}