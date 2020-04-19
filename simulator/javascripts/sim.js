//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0

const WEBPAGE_VERSION = true;
//simulation inputs
const INPUTPATHPREFIX = 'staticInst/data/web_input_files';
var cityName = 'bengaluru';
var inputPath = INPUTPATHPREFIX + '/' + cityName + '/';

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
const LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI = 6
const LOCKDOWN_21 = 7
const LD_21_CI_HQ_SD70_SC_21_SC_42 = 8
const LD_21_CI_HQ_SD70_SC_21 = 9
const LD_21_CI_HQ_SD70_SC_OE_30 = 10

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

let csvContent = "data:text/csv;charset=utf-8,"; //for file dump

INCUBATION_PERIOD = 2.3
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

//some required functions
COMPLIANCE_PROBABILITY = set_compliance();

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
        case LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI:
            val = 0.9;
            break;
        case LOCKDOWN_21:
            val = 0.9;
            break;
        case LD_21_CI_HQ_SD70_SC_21_SC_42:
            val = 0.9;
            break;
        case LD_21_CI_HQ_SD70_SC_21:
            val = 0.9;
            break;
        case LD_21_CI_HQ_SD70_SC_OE_30:
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
//Data can be taken from a json file.
function compute_prob_infection_given_community(infection_probability, set_uniform) {

    var prob_infec_given_community = [];
    var communities_population_json = JSON.parse(loadJSON_001(inputPath + 'fractionPopulation.json'));
    if (!set_uniform) {
        var communities_frac_quarantined_json = JSON.parse(loadJSON_001(inputPath + 'quarantinedPopulation.json'));
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
    return prob_infec_given_community;
}


function get_age_group(age) {
    var age_group = 0;
    age_group = Math.floor(age / 5);
    if (age > 79) {
        age_group = 15;
    }
    return age_group;
}

// Initialise the nodes with various features.
function init_nodes() {

    var individuals_json = JSON.parse(loadJSON_001(inputPath + 'individuals.json'));
    var workplace_json = JSON.parse(loadJSON_001(inputPath + 'workplaces.json'));
    //console.log(individuals_json.length,individuals_json[0]);
    NUM_PEOPLE = individuals_json.length;
    NUM_WORKPLACES = workplace_json.length;
    //console.log("Num People", NUM_PEOPLE, "Num Workspaces",NUM_WORKPLACES)
    if (SEEDING_MODE == RANDOM_SEEDING_WARDWISE) {
        COMMUNITY_INFECTION_PROB = compute_prob_infection_given_community(INIT_FRAC_INFECTED, SEED_WARD_FRACTION_UNIFORM);
    }
    //console.log(COMMUNITY_INFECTION_PROB)
    var nodes = [];
    var stream1 = new Random(1234);

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
    var infection_seeds_json = JSON.parse(loadJSON_001(inputPath + 'infection_seeds.json'));
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
    console.log(curr_time / SIM_STEPS_PER_DAY, num_individuals_to_seed);
}

function infection_seeding_exp_rate(nodes, curr_time) {
    if (curr_time >= SEEDING_START_DATE * SIM_STEPS_PER_DAY && curr_time < (SEEDING_START_DATE + SEEDING_DURATION) * SIM_STEPS_PER_DAY) {
        var time_since_seeding_start = curr_time - SEEDING_START_DATE * SIM_STEPS_PER_DAY;
        var seed_doubling_time = SEEDING_DOUBLING_TIME * SIM_STEPS_PER_DAY;
        var current_seeding_rate = SEEDING_RATE_SCALE * Math.pow(2, time_since_seeding_start / seed_doubling_time);
        var num_seeds_curr_time = d3.randomPoisson(current_seeding_rate)();
        var num_seeded = 0;
        console.log(curr_time, num_seeds_curr_time);
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

/*
function get_individuals_at_home(nodes, h){
	var individuals = []
	for (var i=0; i<NUM_PEOPLE; i++){
		if (nodes[i]['home']==h){
			individuals.push(i)
		}
	}
	return individuals;
}

function get_individuals_at_workplace(nodes, w){
	var individuals = []
	for (var i=0; i<NUM_PEOPLE; i++){
		if (nodes[i]['workplace']==w){
			individuals.push(i)
		}
	}
	return individuals;
}

function get_individuals_at_community(nodes, c){
	var individuals = []
	for (var i=0; i<NUM_PEOPLE; i++){
		if (nodes[i]['community']==c){
			individuals.push(i)
		}
	}
	return individuals;
}
*/

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
        console.log(tr, public_transports[tr]['scale']);
    }

}


//Functions to init homes, workplaces and communities

function init_homes() {

    var houses_json = JSON.parse(loadJSON_001(inputPath + 'houses.json'));
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
    var workplaces_json = JSON.parse(loadJSON_001(inputPath + 'workplaces.json'));
    var schools_json = JSON.parse(loadJSON_001(inputPath + 'schools.json'));
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

    var communities_json = JSON.parse(loadJSON_001(inputPath + 'commonArea.json'));
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
    var inter_ward_distances_json = JSON.parse(loadJSON_001(inputPath + 'wardCentreDistance.json'));

    var community_dist_matrix = math.zeros([communities.length, communities.length]);
    /// console.log(community_dist_matrix)
    /*
     for (var c1 =0; c1< communities.length;c1++){
         for (var c2=c1+1; c2<communities.length;c2++){
            /// console.log(communities[c1]['loc'],communities[c2]['loc'])
             community_dist_matrix[c1][c2] = euclidean(communities[c1]['loc'],communities[c2]['loc']);
             community_dist_matrix[c2][c1] = community_dist_matrix[c1][c2];

         }
     }
    */

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
        console.log("Home: ", h, " - ", homes[h]['individuals'].length, ". Working individuals  = ", working_count);
    }
    for (var h = 0; h < workplaces.length; h++) {
        console.log("workplace: ", h, " - ", workplaces[h]['individuals'].length)
    }
    for (var h = 0; h < communities.length; h++) {
        console.log("Community: ", h, " - ", communities[h]['individuals'].length)
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


    if (node['infection_status'] == SUSCEPTIBLE && Math.random() < (1 - Math.exp(-node['lambda'] / SIM_STEPS_PER_DAY))) {
        node['infection_status'] = EXPOSED; //move to exposed state
        node['time_of_infection'] = cur_time;
        node['infective'] = 0;
        update_lambda_stats(node)
    } else if (node['infection_status'] == EXPOSED && (cur_time - node['time_of_infection'] >= node['incubation_period'])) {
        node['infection_status'] = PRE_SYMPTOMATIC;//move to infective state
        node['infective'] = 1;
    } else if (node['infection_status'] == PRE_SYMPTOMATIC && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period']))) {
        if (Math.random() < SYMPTOMATIC_FRACTION) {
            node['infection_status'] = SYMPTOMATIC;//move to symptomatic
            node['infective'] = 1;
            NUM_AFFECTED_COUNT++;
        } else {
            node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
        }
    } else if (node['infection_status'] == SYMPTOMATIC && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period'] + node['symptomatic_period']))) {
        if (Math.random() < STATE_TRAN[age_index][0]) {
            node['infection_status'] = HOSPITALISED;//move to hospitalisation
            node['infective'] = 0;
        } else {
            node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
        }
    } else if (node['infection_status'] == HOSPITALISED && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period'] + node['symptomatic_period'] + node['hospital_regular_period']))) {
        if (Math.random() < STATE_TRAN[age_index][1]) {
            node['infection_status'] = CRITICAL;//move to critical care
            node['infective'] = 0;
        } else {
            node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
        }
    } else if (node['infection_status'] == CRITICAL && (cur_time - node['time_of_infection'] >= (node['incubation_period'] + node['asymptomatic_period'] + node['symptomatic_period'] + node['hospital_regular_period'] + node['hospital_critical_period']))) {
        if (Math.random() < STATE_TRAN[age_index][2]) {
            node['infection_status'] = DEAD;//move to dead
            node['infective'] = 0;
        } else {
            node['infection_status'] = RECOVERED;//move to recovered
            node['infective'] = 0;
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

    var sigma_json = JSON.parse(loadJSON_001(inputPath + 'Sigma_household.json'));
    var U_matrix_json = JSON.parse(loadJSON_001(inputPath + 'U_household.json'));
    var V_matrix_json = JSON.parse(loadJSON_001(inputPath + 'Vtranspose_household.json'));

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
    var sigma_json = JSON.parse(loadJSON_001(inputPath + 'Sigma_workplace.json'));
    var U_matrix_json = JSON.parse(loadJSON_001(inputPath + 'U_workplace.json'));
    var V_matrix_json = JSON.parse(loadJSON_001(inputPath + 'Vtranspose_workplace.json'));

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

    var sigma_json = JSON.parse(loadJSON_001(inputPath + 'Sigma_school.json'));
    var U_matrix_json = JSON.parse(loadJSON_001(inputPath + 'U_school.json'));
    var V_matrix_json = JSON.parse(loadJSON_001(inputPath + 'Vtranspose_school.json'));

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


let csvContent_alltogether = "data:text/csv;charset=utf-8,";

function update_all_kappa(nodes, homes, workplaces, communities, cur_time) {
    var current_time = cur_time;
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
            case LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI:
                get_kappa_LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI(nodes, homes, workplaces, communities, current_time);
                break;
            case LOCKDOWN_21:
                get_kappa_LOCKDOWN_21(nodes, homes, workplaces, communities, current_time);
                break;
            case LD_21_CI_HQ_SD70_SC_21_SC_42:
                get_kappa_LD_21_CI_HQ_SD70_SC_21_SC_42(nodes, homes, workplaces, communities, current_time);
                break;
            case LD_21_CI_HQ_SD70_SC_21:
                get_kappa_LD_21_CI_HQ_SD70_SC_21(nodes, homes, workplaces, communities, current_time);
                break;
            case LD_21_CI_HQ_SD70_SC_OE_30:
                get_kappa_LD_21_CI_HQ_SD70_SC_OE_30(nodes, homes, workplaces, communities, current_time);
                break;
            default:
                break;
        }
    }
}


function run_simday(time_step, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
                    days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution) {

    console.log(time_step / SIM_STEPS_PER_DAY);

    //seeding strategies
    if (SEEDING_MODE == SEED_INFECTION_RATES && time_step < seed_array.length) {
        infection_seeding(nodes, seed_array[time_step], time_step);
    } else if (SEEDING_MODE == SEED_EXP_RATE) {
        infection_seeding_exp_rate(nodes, time_step);
    }

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

        
        let row = [time_step / SIM_STEPS_PER_DAY, c, temp_stats[0], temp_stats[1], temp_stats[2], temp_stats[3], temp_stats[4]].join(",");
        csvContent += row + "\r\n";
    }
    for (var pt = 0; pt < NUM_PUBLIC_TRANSPORT; pt++) {
        public_transports[pt]['lambda_PT'] = update_lambda_public_transport(nodes, public_transports[pt]);
    }


    update_lambda_c_global(communities, community_distance_matrix);


    for (var j = 0; j < NUM_PEOPLE; j++) {
        var lambda_current_stats = [];
        update_lambdas(nodes[j], homes, workplaces, communities, public_transports, nodes, time_step);
        
    }

    //lambda_current_stats_avg = math.mean(math.mean(lambda_current_stats, 0));
    //lambda_evolution.push([i/SIM_STEPS_PER_DAY,lambda_current_stats_avg[2],lambda_current_stats_avg[3],lambda_current_stats_avg[4],lambda_current_stats_avg[5]]);

    var n_infected_wardwise = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == PRE_SYMPTOMATIC || node['infection_status'] == SYMPTOMATIC || node['infection_status'] == HOSPITALISED || node['infection_status'] == CRITICAL) ? 1 : 0);
    }, 0);
    //I don't know what to do with this. It is not an initialized variable at start. Thus i'm commenting out
    // days_num_infected.push([time_step / SIM_STEPS_PER_DAY, n_infected]);
    csvContent_ninfected = [time_step / SIM_STEPS_PER_DAY, n_infected].join(',') + "\r\n"

    var n_infected = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == PRE_SYMPTOMATIC || node['infection_status'] == SYMPTOMATIC || node['infection_status'] == HOSPITALISED || node['infection_status'] == CRITICAL) ? 1 : 0);
    }, 0);
    //PLEASE CHECK BELOW LINE AS  n_infected_wardwise returns same thing
    days_num_infected.push([time_step / SIM_STEPS_PER_DAY, n_infected]);

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
    days_num_fatalities.push([time_step / SIM_STEPS_PER_DAY, (n_fatalities)]);

    var n_recovered = nodes.reduce(function (partial_sum, node) {
        return partial_sum + ((node['infection_status'] == RECOVERED) ? 1 : 0);
    }, 0);
    days_num_recovered.push([time_step / SIM_STEPS_PER_DAY, n_recovered]);

    //var n_affected = nodes.reduce(function(partial_sum, node) {return partial_sum + ((node['infection_status']) ? 1 : 0);}, 0);
    days_num_affected.push([time_step / SIM_STEPS_PER_DAY, (NUM_AFFECTED_COUNT)]);

    let row = [time_step / SIM_STEPS_PER_DAY, NUM_AFFECTED_COUNT, n_recovered, n_infected, n_exposed, n_hospitalised, n_critical, n_fatalities, LAMBDA_INFECTION_MEAN[0], LAMBDA_INFECTION_MEAN[1], LAMBDA_INFECTION_MEAN[2]].join(",");
    csvContent_alltogether += row + "\r\n";
    if (LAMBDA_INFECTION_STATS.length > 0) {
        lambda_evolution.push([time_step / SIM_STEPS_PER_DAY, [LAMBDA_INFECTION_MEAN[0], LAMBDA_INFECTION_MEAN[1], LAMBDA_INFECTION_MEAN[2], LAMBDA_INFECTION_MEAN[3]]])
    }

    ///update_sim_progress_status(time_step,NUM_TIMESTEPS);

    // }
    if (LAMBDA_INFECTION_STATS.length > 0) {
        console.log(math.mean(LAMBDA_INFECTION_STATS, 0));
    }
    if (time_step % SIM_STEPS_PER_DAY == 0 && time_step > 0) {
        document.getElementById("status").innerHTML = "Simulation Complete for " + time_step / SIM_STEPS_PER_DAY + " Days";
        var statusDiv = document.getElementById("status");
        statusDiv.style.visibility="visible";
        return time_step;
        // return [time_step, days_num_infected, days_num_exposed, days_num_hospitalised, days_num_critical, days_num_fatalities, days_num_recovered, days_num_affected, lambda_evolution];
    } else {
        return run_simday(time_step + 1, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
            days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution);
    }


}

function newFilledArray(len, val) {
    var rv = new Array(len);
    while (--len >= 0) {
        rv[len] = val;
    }
    return rv;
}


function initialize_simulation() {
    document.getElementById("status").innerHTML = "Initializing Simulation...";
    const homes = init_homes();
    const workplaces = init_workplaces();
    const communities = init_community();
    const public_transports = init_public_transport();
    const nodes = init_nodes();
    const community_distance_matrix = compute_community_distances(communities);
    let seed_array = [];
    if (SEEDING_MODE == SEED_INFECTION_RATES) {
        seed_array = load_infection_seed_json(SEED_SCALING_FACTOR);
    }


    
    console.log(NUM_PEOPLE, NUM_HOMES, NUM_WORKPLACES, NUM_SCHOOLS, NUM_COMMUNITIES)

    assign_individual_home_community(nodes, homes, workplaces, communities);
    assign_individual_public_transports(nodes, public_transports, workplaces);
    //compute_individual_community_distance(nodes,communities); //TODO: remove once distance to community center is provided in json.

    compute_scale_homes(homes);
    compute_scale_workplaces(workplaces);
    compute_scale_communities(nodes, communities);
    compute_scale_public_transport(nodes, public_transports);

    //get_init_stats(nodes,homes,workplaces,communities);
    const array_len = NUM_TIMESTEPS;
    const array_default = 0;
    const days_num_affected = newFilledArray(array_len, array_default);
    const days_num_critical = newFilledArray(array_len, array_default);
    const days_num_exposed = newFilledArray(array_len, array_default);
    const days_num_fatalities = newFilledArray(array_len, array_default);
    const days_num_hospitalised = newFilledArray(array_len, array_default);
    const days_num_infected = newFilledArray(array_len, array_default);
    const days_num_recovered = newFilledArray(array_len, array_default);

    let lambda_evolution = []
    LAMBDA_INFECTION_STATS = [] //global variable to track lambda evolution when a person gets infected
    LAMBDA_INFECTION_MEAN = [0, 0, 0, 0];
    NUM_AFFECTED_COUNT = 0;
    document.getElementById("status").innerHTML = "Initialization Done...";
    return [homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution];

}

SIMULATION_STOP = false;

function stop_sim() {
    SIMULATION_STOP = true;
}

function run_simulation() {
    document.getElementById("run_button").style.display = "none";
    document.getElementById("sim_stop").style.display = "inline";


    const [homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution] = initialize_simulation();
    document.getElementById("status").innerHTML = "Starting to run Simulation...";
    let time_step = 0;
    time_step = run_simday(time_step, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution);

    let plot_tuple = [days_num_infected, days_num_exposed, days_num_hospitalised, days_num_critical, days_num_fatalities, days_num_recovered, days_num_affected, lambda_evolution];
    call_plotly(plot_tuple);


    const interval = setInterval(function () {
        console.log("inside the interval stuff. time_step = ", time_step);
        document.getElementById("status").innerHTML = "Calculating Simulation for Day: " + time_step / SIM_STEPS_PER_DAY;
        time_step = run_simday(time_step + 1, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
            days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution);
        call_plotly(plot_tuple);
        // Plotly.extendTraces('graph', {
        //     x: [[cnt], [cnt]],
        //     y: [[rand()], [rand()]]
        // }, [0, 1])

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
            document.getElementById("status").innerHTML = "Numbers plotted are per " + String(NUM_PEOPLE) + ".";
            if (!WEBPAGE_VERSION) {
                link.click();	//TODO: Instead of click link, add link for download on page.
            }

        }
    }, 150);

    if (LAMBDA_INFECTION_STATS.length > 0) {
        console.log(math.mean(LAMBDA_INFECTION_STATS, 0));
    }


}


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

    var layout = {
        autosize: true,
        width: 350,
        height: 300,
        barmode: 'stack',
        margin: {l:50, r:50, t:50, b:50},
        title: {
            text: title_text,
            font: {
                family: 'Courier New, monospace',
                size: 16
            },
            xref: 'paper',
            x: 0.05,
        },
        xaxis: {
            title: {
                text: 'Days',
                font: {
                    family: 'Courier New, monospace',
                    size: 16,
                    color: '#555555'
                }
            },
        },
        yaxis: {
            title: {
                text: title_text,
                font: {
                    family: 'Courier New, monospace',
                    size: 16,
                    color: '#555555'
                }
            },
            range: [0, 1]
        },
        showlegend: true,
        legend: {
            x: 1,
            xanchor: 'right',
            y: 1
        }
    };

    const btnRemove = {modeBarButtonsToRemove: ['hoverClosestCartesian', 'hoverCompareCartesian'] };
    Plotly.newPlot(plot_position, data_plot, layout);
}


function call_plotly(data_tuple) {
    var plot_values = data_tuple;

    plot_plotly([plot_values[0]], 'num_infected_plot_2', 'Number Infectious (daily)', 'Evolution of Infected Population');
    //plot_plotly([plot_values[1]],'num_exposed_plot_2','Number Exposed (daily)','Evolution of Exposed Population');
    plot_plotly([plot_values[2]], 'num_hospitalised_plot_2', 'Number Hospitalised (daily)', 'Evolution of Hospitalised Population');
    plot_plotly([plot_values[3]], 'num_critical_plot_2', 'Number Critical (daily)', 'Evolution of Critical Population');
    plot_plotly([plot_values[4]], 'num_fatalities_plot_2', 'Number Fatalities (cum.)', 'Evolution of Fatalities Population');
    //plot_plotly([plot_values[5]],'num_recovered_plot_2','Number Recovered (cum.)','Evolution of Recovered Population');
    plot_plotly([plot_values[6]], 'num_affected_plot_2', 'Number Affected (cum.)', 'Evolution of Affected Population');
    plot_lambda_evolution([plot_values[7]], 'lambda_evolution', 'Source of infection', ['Home', 'School/Workplace', 'Community', 'Public Transport']);

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
    'num_affected_plot_2' : 100,
    'num_infected_plot_2': 50,
    'num_hospitalised_plot_2': 20,
    'num_critical_plot_2': 10,
    'num_fatalities_plot_2': 5
};

function plot_plotly(data, plot_position, title_text, legends) {
    const trace = [];

    for (var count = 0; count < data.length; count++) {
        var trace1 = {
            x: [],
            y: [],
            mode: 'lines',
            name: legends[count],
            line: {
                // color: 'rgb(219, 64, 82)',
                width: 3
            }
        };
        for (var count2 = 0; count2 < data[count].length; count2++) {
            if ( Array.isArray(data[count][count2]) ) {
                trace1.x.push(data[count][count2][0]);
                trace1.y.push(data[count][count2][1]);
            }
        }
        trace.push(trace1)
    }

    const data_plot = trace;

    const yMax = Math.max(...data_plot[0].y);

    const layout = {
        autosize: true,
        width: 350,
        height:300,
        margin: {l:50, r:50, t:50, b:50},
        xaxis: {
            title: {
                text: 'Days',
                font: {
                    family: 'Courier New, monospace',
                    size: 16,
                    color: '#555555'
                }
            }
        },
        yaxis: {
            title: {
                text: title_text,
                font: {
                    family: 'Courier New, monospace',
                    size: 16,
                    color: '#555555'
                }
            }
        }
    };

    if (yMax*1.3 <= plot_minRanges[plot_position] ) {
        layout.yaxis.range = [0, plot_minRanges[plot_position]];
    }
    else {
        layout.yaxis.autorange = true;
        //layout.yaxis.range = [0, Math.max(plot_minRanges[plot_position], yMax*2.0)];
    }

    const btnRemove = {modeBarButtonsToRemove: ['hoverClosestCartesian', 'hoverCompareCartesian'] };
    Plotly.newPlot(plot_position, data_plot, layout, btnRemove);
}

//Main function
function runSimulations() {

    //clear_plots();

    //get the inputs from the HTML page
    NUM_DAYS = document.getElementById("numDays").value; // == 0 ? NUM_DAYS : document.getElementById("numDays").value;
    NUM_TIMESTEPS = NUM_DAYS * SIM_STEPS_PER_DAY;

    // initFrac is actually number Exposed, now input as percentage instead of fraction
    INIT_FRAC_INFECTED = parseFloat(document.getElementById("initFrac").value) / 100.0;
//	COMPLIANCE_PROBABILITY = document.getElementById("compliance").value;

    INCUBATION_PERIOD = parseFloat(document.getElementById("Incubation").value) / 2;
    INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD * SIM_STEPS_PER_DAY; // 2.29 days


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

    //INTERVENTION = parseInt(document.getElementById("interventions").value);
    INTERVENTION = parseInt(document.querySelector('input[name="interventions2"]:checked').value);

    console.log(NUM_DAYS, INIT_FRAC_INFECTED, INTERVENTION);
    console.log("INTERVENTION = ", INTERVENTION);

    //where simulation starts
    run_simulation();


}


function clear_plots() {
    //clear previous plots
    document.getElementById("status").innerHTML = "Simulation in Progress....";
    document.getElementById("num_affected_plot_2").innerHTML = "";
    document.getElementById("num_infected_plot_2").innerHTML = "";
    document.getElementById("num_exposed_plot_2").innerHTML = "";
    document.getElementById("num_hospitalised_plot_2").innerHTML = "";
    document.getElementById("num_critical_plot_2").innerHTML = "";
    document.getElementById("num_fatalities_plot_2").innerHTML = "";
    document.getElementById("num_recovered_plot_2").innerHTML = "";
    document.getElementById("lambda_evolution").innerHTML = "";

    runSimulations();
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
    document.getElementById("compliance").value = COMPLIANCE_PROBABILITY;
    document.getElementById("betaHouse").value = BETA_H;
    document.getElementById("betaWork").value = BETA_W;
    document.getElementById("betaCommunity").value = BETA_C;
    document.getElementById("betaSchools").value = BETA_S;
    document.getElementById("betaPT").value = BETA_PT;
    //document.getElementById("interventions").value = "0";
}

function clear_variables() {

}

set_default_values_html();
