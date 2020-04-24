var InitSimulation = function(compute){
   this.compute = compute;

}
InitSimulation.prototype.init_workplaces = function(){
    var workplaces_json = JSON.parse(loadJSON_001('input_files/workplaces.json'));
    var schools_json = JSON.parse(loadJSON_001('input_files/schools.json'));
    // console.log("In init workplaces:",workplaces_json.length,workplaces_json[0]);
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
        //workplace['scale'] = BETA_W*workplace['Q_w']/workplace['individuals'].length;
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
        //workplace['scale'] = BETA_W*workplace['Q_w']/workplace['individuals'].length;
        workplaces.push(workplace)
    }
    if (USE_AGE_DEPENDENT_MIXING) {
        get_age_dependent_mixing_matrix_workplace();
    }

    return workplaces;
}
InitSimulation.prototype.set_compliance=function() {
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
InitSimulation.prototype.compliance = function() {
    var val = (Math.random() < this.set_compliance()) ? 1 : 0;
    return val;
}

InitSimulation.prototype.init_homes = function () {
    var houses_json = JSON.parse(loadJSON_001('input_files/houses.json'));
    // console.log("In init homes:",houses_json.length,houses_json[0]);
    NUM_HOMES = houses_json.length;

    var homes = [];
    for (var h = 0; h < NUM_HOMES; h++) {
        var home = {
            'loc': [houses_json[h]['lat'], houses_json[h]['lon']], // [lat, long],
            'lambda_home': 0,
            'individuals': [], // We will populate this later
            'Q_h': 1,
            'scale': 0,
            'compliant': this.compliance(),
            'quarantined': false,
            'age_dependent_mixing': math.zeros([NUM_AGE_GROUPS])
        };
        //home['scale'] = BETA_H*home['Q_h']/(Math.pow(home['individuals'].length, ALPHA));

        homes.push(home)

    }
    if (USE_AGE_DEPENDENT_MIXING) {
        this.get_age_dependent_mixing_matrix_household(); //get age dependent mixing matrix for households.
    }
    return homes;
};
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

InitSimulation.prototype.init_community =  function(){

    var communities_json = JSON.parse(loadJSON_001('input_files/commonArea.json'));
    // console.log("In init community",communities_json.length,communities_json);
    communities_json.sort(compare_wards);
    // console.log("In init community",communities_json.length,communities_json[0]['location']);
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

InitSimulation.prototype.init_public_transport = function() {

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




InitSimulation.prototype.init_nodes = function() {

    var individuals_json = JSON.parse(loadJSON_001('input_files/individuals.json'));
    var workplace_json = JSON.parse(loadJSON_001('input_files/workplaces.json'));
    //console.log(individuals_json.length,individuals_json[0]);
    NUM_PEOPLE = individuals_json.length;
    NUM_WORKPLACES = workplace_json.length;
    //console.log("Num People", NUM_PEOPLE, "Num Workspaces",NUM_WORKPLACES)
    if (SEEDING_MODE == RANDOM_SEEDING_WARDWISE) {
        COMMUNITY_INFECTION_PROB = this.compute_prob_infection_given_community(INIT_FRAC_INFECTED, SEED_WARD_FRACTION_UNIFORM);
    }
    //console.log(COMMUNITY_INFECTION_PROB)
    var nodes = [];
    var stream1 = new Random(1234);

    for (var i = 0; i < NUM_PEOPLE; i++) {

        //console.log(stream1.gamma(1,1))

        var node = {
            'loc': [individuals_json[i]['lat'], individuals_json[i]['lon']], // [lat, long]
            'age': individuals_json[i]['age'],
            'age_group': this.get_age_group(individuals_json[i]['age']),
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
            'funct_d_ck': this.compute.f_kernel(individuals_json[i]['CommunityCentreDistance']), // TODO: need to use the kernel function. function of distance from community...f_kernel
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
        node['zeta_a'] = this.compute.zeta(node['age']);

        if (SEEDING_MODE == SEED_FROM_INDIVIDUALS_JSON) {
            node['infection_status'] = individuals_json[i]['infection_status'];
            node['time_of_infection'] = node['infection_status'] == EXPOSED ? (individuals_json[i]['time_of_infection'] * SIM_STEPS_PER_DAY - node['incubation_period']) : 0;
            /*if(node['infection_status']==EXPOSED){
                //console.log(node['time_of_infection']);
            }*/
        } else if (SEEDING_MODE == RANDOM_SEEDING_WARDWISE) {
            node['infection_status'] = (Math.random() < COMMUNITY_INFECTION_PROB[individuals_json[i]['wardNo'] - 1]) ? 1 : 0
        }
        nodes.push(node)

    }
    return nodes;
}

InitSimulation.prototype.load_infection_seed_json=function(seed_scaling_factor) {
    var seed_array = [];
    var infection_seeds_json = JSON.parse(loadJSON_001('input_files/infection_seeds.json'));
    var infection_seeds_json_array = Object.values(infection_seeds_json['seed_fit']);
    var num_seed_days = infection_seeds_json_array.length;
    for (var count = 0; count < num_seed_days * SIM_STEPS_PER_DAY; count++) {
        var mean = (infection_seeds_json_array[Math.floor(count / SIM_STEPS_PER_DAY)] / SIM_STEPS_PER_DAY) * seed_scaling_factor;
        //var random = d3.randomPoisson(mean);
        //console.log(mean);
        seed_array.push(d3.randomPoisson(mean)());
    }
    return seed_array;
}
InitSimulation.prototype.get_age_dependent_mixing_matrix_workplace=function() {
    var sigma_json = JSON.parse(loadJSON_001('input_files/Sigma_workplace.json'));
    var U_matrix_json = JSON.parse(loadJSON_001('input_files/U_workplace.json'));
    var V_matrix_json = JSON.parse(loadJSON_001('input_files/Vtranspose_workplace.json'));

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

    var sigma_json = JSON.parse(loadJSON_001('input_files/Sigma_school.json'));
    var U_matrix_json = JSON.parse(loadJSON_001('input_files/U_school.json'));
    var V_matrix_json = JSON.parse(loadJSON_001('input_files/Vtranspose_school.json'));

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
InitSimulation.prototype.get_age_group=function(age) {
    var age_group = 0;
    age_group = Math.floor(age / 5);
    if (age > 79) {
        age_group = 15;
    }
    return age_group;
}
InitSimulation.prototype.compute_prob_infection_given_community=function(infection_probability, set_uniform) {

    var prob_infec_given_community = [];
    var communities_population_json = JSON.parse(loadJSON_001('input_files/fractionPopulation.json'));
    if (!set_uniform) {
        var communities_frac_quarantined_json = JSON.parse(loadJSON_001('input_files/quarantinedPopulation.json'));
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
InitSimulation.prototype.get_init_stats=function(nodes, homes, workplaces, communities) {
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
InitSimulation.prototype.get_age_dependent_mixing_matrix_household=function() {

    var sigma_json = JSON.parse(loadJSON_001('Sigma_household.json'));
    var U_matrix_json = JSON.parse(loadJSON_001('U_household.json'));
    var V_matrix_json = JSON.parse(loadJSON_001('Vtranspose_household.json'));

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

