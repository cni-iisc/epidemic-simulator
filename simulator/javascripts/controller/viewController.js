
const WEBPAGE_VERSION = true;
//simulation inputs


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


INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD * SIM_STEPS_PER_DAY; // 2.29 days
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

var SIMULATION_STOP = false;
const RADIUS_EARTH = 6371; ///km

const plot_minRanges = {
    'num_affected_plot_2' : 100,
    'num_infected_plot_2': 50,
    'num_hospitalised_plot_2': 20,
    'num_critical_plot_2': 10,
    'num_fatalities_plot_2': 5
};

function newFilledArray(len, val) {
    var rv = new Array(len);
    while (--len >= 0) {
        rv[len] = val;
    }
    return rv;
}
function initialize_simulation(){
    document.getElementById("status").innerHTML = "Initializing Simulation...";
    var computeSimulatorObj = new compute();
    var initSimulatorObj= new InitSimulation(computeSimulatorObj);
   
    const homes = initSimulatorObj.init_homes();
    const workplaces = initSimulatorObj.init_workplaces();
    const communities = initSimulatorObj.init_community();
    const public_transports = initSimulatorObj.init_public_transport();
    const nodes = initSimulatorObj.init_nodes();
    const community_distance_matrix = computeSimulatorObj.compute_community_distances(communities);
    let seed_array = [];
    if (SEEDING_MODE == SEED_INFECTION_RATES) {
        seed_array = initSimulatorObj.load_infection_seed_json(SEED_SCALING_FACTOR);
    }


    //console.log(community_distance_matrix)
    console.log(NUM_PEOPLE, NUM_HOMES, NUM_WORKPLACES, NUM_SCHOOLS, NUM_COMMUNITIES)

    computeSimulatorObj.assign_individual_home_community(nodes, homes, workplaces, communities);
    computeSimulatorObj.assign_individual_public_transports(nodes, public_transports, workplaces);
    //compute_individual_community_distance(nodes,communities); //TODO: remove once distance to community center is provided in json.

    computeSimulatorObj.compute_scale_homes(homes);
    computeSimulatorObj.compute_scale_workplaces(workplaces);
    computeSimulatorObj.compute_scale_communities(nodes, communities);
    computeSimulatorObj.compute_scale_public_transport(nodes, public_transports);

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

function runSimulation(){
    
    var computeObj = new compute();
    const [homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution] = initialize_simulation();
    document.getElementById("status").innerHTML = "Starting to run Simulation...";
    let time_step = 0;
    time_step = computeObj.run_simday(time_step, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
        days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution);

    let plot_tuple = [days_num_infected, days_num_exposed, days_num_hospitalised, days_num_critical, days_num_fatalities, days_num_recovered, days_num_affected, lambda_evolution];
    call_plotly(plot_tuple);
    document.getElementById("in-progress").style.display = 'none';
    document.getElementById('plots-area').style.display = 'block';

    const interval = setInterval(function () {
        console.log("inside the interval stuff. time_step = ", time_step);
        document.getElementById("status").innerHTML = "Calculating Simulation for Day: " + time_step / SIM_STEPS_PER_DAY;
        time_step = computeObj.run_simday(time_step + 1, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
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


function getData(){

    NUM_DAYS = document.getElementById("numDays").value; // == 0 ? NUM_DAYS : document.getElementById("numDays").value;
    NUM_TIMESTEPS = NUM_DAYS * SIM_STEPS_PER_DAY;

    INIT_FRAC_INFECTED = document.getElementById("initFrac").value;
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

    //Ppatil
    //INTERVENTION = parseInt(document.getElementById("interventions").value);
    //INTERVENTION = parseInt(document.getElementById("interventions2").value);
    INTERVENTION = parseInt(document.querySelector('input[name="interventions2"]:checked').value);

    console.log(NUM_DAYS, INIT_FRAC_INFECTED, INTERVENTION);
    console.log("INTERVENTION = ", INTERVENTION);
    document.getElementById("run_button").style.display = "none";
    document.getElementById("sim_stop").style.display = "inline";
    runSimulation();
    

}

function clear_plots() {
    require(['model/constants'], function(result){
       console.log("hey");
    });
    //var x = require('../model/constants.js');
    
    document.getElementById("no-data").style.display = 'none';
    document.getElementById('plots-area').style.display = 'none';
    document.getElementById("in-progress").style.display = 'block';
    document.getElementById("status").innerHTML = "Simulation in Progress....";
    document.getElementById("status").style.display = 'inline'
    document.getElementById("num_affected_plot_2").innerHTML = "";
    document.getElementById("num_infected_plot_2").innerHTML = "";
    document.getElementById("num_exposed_plot_2").innerHTML = "";
    document.getElementById("num_hospitalised_plot_2").innerHTML = "";
    document.getElementById("num_critical_plot_2").innerHTML = "";
    document.getElementById("num_fatalities_plot_2").innerHTML = "";
    document.getElementById("num_recovered_plot_2").innerHTML = "";
    document.getElementById("lambda_evolution").innerHTML = "";
    setTimeout(function() {
    getData();
},0)

    //runSimulations();
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

    //Ppatil
    const yMax = Math.max(...data_plot[0].y);

    const layout = {
        autosize: true,
        width: 400,
        height:400,
        xaxis: {
            title: {
                text: 'Days',
                font: {
                    family: 'Courier New, monospace',
                    size: 18,
                    color: '#7f7f7f'
                }
            }
        },
        yaxis: {
            title: {
                text: title_text,
                font: {
                    family: 'Courier New, monospace',
                    size: 18,
                    color: '#7f7f7f'
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

    Plotly.newPlot(plot_position, data_plot, layout);
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
        width: 400,
        height: 400,
        barmode: 'stack',
        title: {
            text: title_text,
            font: {
                family: 'Courier New, monospace',
                size: 24
            },
            xref: 'paper',
            x: 0.05,
        },
        xaxis: {
            title: {
                text: 'Days',
                font: {
                    family: 'Courier New, monospace',
                    size: 18,
                    color: '#7f7f7f'
                }
            },
        },
        yaxis: {
            title: {
                text: title_text,
                font: {
                    family: 'Courier New, monospace',
                    size: 18,
                    color: '#7f7f7f'
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


    Plotly.newPlot(plot_position, data_plot, layout);
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

function stop_sim() {
    SIMULATION_STOP = true;
}

function citySelectionChange(event){
    var inputControl = document.getElementById("Wards");
    inputControl.readonly= false
    switch(event.value){
        case 'Mumbai':
            inputControl.value = 227;
            break;
        case 'Bengaluru':
            inputControl.value = 198;
            break;  
    }
    inputControl.readonly= true
}


