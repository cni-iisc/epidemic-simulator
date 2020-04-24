var compute = function(){

}

compute.prototype.compute_community_distances= function(communities) {
    var inter_ward_distances_json = JSON.parse(loadJSON_001('input_files/wardCentreDistance.json'));

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
            /// console.log(communities[c1]['loc'],communities[c2]['loc'])
            community_dist_matrix[c1][c2] = inter_ward_distances_json[c1][(c2 + 1).toString()];
            community_dist_matrix[c2][c1] = community_dist_matrix[c1][c2];

        }
    }


    return community_dist_matrix;
}

compute.prototype.assign_individual_home_community = function(nodes, homes, workplaces, communities) {
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
compute.prototype.assign_individual_public_transports= function(nodes, public_transports, workplaces) {
    //Assign individuals to homes, workplace, community
    for (var i = 0; i < nodes.length; i++) {
        if (nodes[i]['public_transport'] != null && (nodes[i]['workplace_type'] == WTYPE_OFFICE || nodes[i]['workplace_type'] == WTYPE_SCHOOL)) {
            public_transports[nodes[i]['public_transport']]['individuals'].push(i); //No checking for null as all individuals have a home
            nodes[i]['dist_hw'] = this.euclidean(nodes[i]['loc'], workplaces[nodes[i]['workplace']]['loc']);
            //console.log("Node: ",i, ". Workplace : ", nodes[i]['workplace'], ". dist_hw: ",nodes[i]['dist_hw'])

        }
    }
}
compute.prototype.compute_scale_homes=function(homes) {

    for (var w = 0; w < homes.length; w++) {
        if (homes[w]['individuals'].length == 0) {
            homes[w]['scale'] = 0;
        } else {
            homes[w]['scale'] = BETA_H * homes[w]['Q_h'] / (Math.pow(homes[w]['individuals'].length, ALPHA));
        }
    }
}

compute.prototype.compute_scale_workplaces = function(workplaces) {
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


compute.prototype.compute_scale_communities = function(nodes, communities) {

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

compute.prototype.compute_scale_public_transport = function(nodes, public_transports) {

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
compute.prototype.infection_seeding_exp_rate = function(nodes, curr_time) {
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
                nodes[individual_index]['time_of_infection'] = curr_time; //*Math.random(); //TODO. Need to revisit.
                num_seeded++;
            }
        }
    }
}

// This is a multiplication factor that quantifies an individual's infective status given the infection state.
compute.prototype.kappa_T = function(node, cur_time) {
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
compute.prototype.psi_T=function(node, cur_time) {
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

compute.prototype.f_kernel=function(d) {
    var a = 10.751 //4. in kms
    var b = 5.384 //3.8. both values are for Thailand, until we get a fit for India
    return 1 / (1 + Math.pow(d / a, b))

}


compute.prototype.zeta=function(age) {
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

compute.prototype.update_individual_lambda_h=function(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * (1 + node['severity']) * node['kappa_H'];
}

compute.prototype.update_individual_lambda_w=function(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * (1 + node['severity'] * (2 * node['psi_T'] - 1)) * node['kappa_W'];
}

compute.prototype.update_individual_lambda_c=function(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * node['funct_d_ck'] * (1 + node['severity']) * node['kappa_C'];
    // optimised version: return node['lambda_h] * node['funct_d_ck'];
}

compute.prototype.update_individual_lambda_pt=function(node) {
    return node['infective'] * node['kappa_T'] * node['infectiousness'] * node['dist_hw'] * (1 + node['severity'] * (2 * node['psi_T'] - 1)) * node['kappa_PT'];
    // optimised version: return node['lambda_h] * node['funct_d_ck'];
}
compute.prototype.update_psi=function(node, cur_time) {
    node['psi_T'] = this.psi_T(node, cur_time);
    //console.log(node['psi_T'])
}


compute.prototype.update_lambda_h_old=function(nodes, home) {
    var sum_value = 0

    for (var i = 0; i < home['individuals'].length; i++) {
        //	var temp = nodes.filter( function(node) {
        //		return node['index']==home['individuals'][i];
        //	});
        sum_value += nodes[home['individuals'][i]]['lambda_h'];
    }
    return home['scale'] * sum_value;
    // Populate it afterwards...
}




compute.prototype.update_lambda_h=function(nodes, home) {
    //Compute age_group related mixing
    var lambda_age_group = math.zeros([NUM_AGE_GROUPS]);

    if (!USE_AGE_DEPENDENT_MIXING) {
        //////////////////////////
        //Sanity test --- use old lambdas
        var lambda_old = this.update_lambda_h_old(nodes, home)


        for (var count = 0; count < NUM_AGE_GROUPS; count++) {
            lambda_age_group[count] = lambda_old;
        }
        ////////////////////////////

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


compute.prototype.update_lambda_w_old=function(nodes, workplace) {
    var sum_value = 0

    for (var i = 0; i < workplace['individuals'].length; i++) {
        //	var temp = nodes.filter( function(node) {
        ///		return node['index']==workplace['individuals'][i];
        //	});
        sum_value += nodes[workplace['individuals'][i]]['lambda_w'];
    }


    return workplace['scale'] * sum_value;
    // Populate it afterwards...
}
compute.prototype.update_lambda_w=function(nodes, workplace) {
    //Compute age_group related mixing
    var lambda_age_group = math.zeros([NUM_AGE_GROUPS]);

    if (!USE_AGE_DEPENDENT_MIXING) {
        //////////////////////////
        //Sanity test --- use old lambdas
        var lambda_old = this.update_lambda_w_old(nodes, workplace)


        for (var count = 0; count < NUM_AGE_GROUPS; count++) {
            lambda_age_group[count] = lambda_old;
        }
        ////////////////////////////

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

compute.prototype.update_lambda_public_transport=function(nodes, public_transport) {
    var sum_value = 0
    for (var i = 0; i < public_transport['individuals'].length; i++) {
        //	var temp = nodes.filter( function(node) {
        //		return node['index']==community['individuals'][i];
        //	});
        sum_value += nodes[public_transport['individuals'][i]]['lambda_pt'];
    }
    return public_transport['scale'] * sum_value;
    // Populate it afterwards...
}


compute.prototype.update_lambda_c_local=function(nodes, community) {
    var sum_value = 0
    for (var i = 0; i < community['individuals'].length; i++) {
        //	var temp = nodes.filter( function(node) {
        //		return node['index']==community['individuals'][i];
        //	});
        sum_value += nodes[community['individuals'][i]]['lambda_c'];
    }
    return community['scale'] * sum_value;
    // Populate it afterwards...
}

compute.prototype.update_lambda_c_global=function(communities, community_distance_matrix) {
    for (var c1 = 0; c1 < communities.length; c1++) {
        var temp = 0;
        var temp2 = 0;
        for (var c2 = 0; c2 < communities.length; c2++) {
            temp += this.f_kernel(community_distance_matrix[c1][c2]) * communities[c2]['lambda_community'];
            temp2 += this.f_kernel(community_distance_matrix[c1][c2]);
            //console.log(c1,c2,f_kernel(community_distance_matrix[c1][c2])*communities[c2]['lambda_community'])
        }
        communities[c1]['lambda_community_global'] = temp / temp2;

    }
}

compute.prototype.get_infected_community=function(nodes, community) {
    var affected_stat = 0
    var infected_stat = 0
    var exposed_stat = 0
    var hospitalised_stat = 0
    var critical_stat = 0
    var dead_stat = 0
    var recovered_stat = 0
    var exposed_stat = 0

    for (var i = 0; i < community['individuals'].length; i++) {
        //	var temp = nodes.filter( function(node) {
        //		return node['index']==community['individuals'][i];
        //	});

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
    // Populate it afterwards...
}

compute.prototype.update_lambdas=function(node, homes, workplaces, communities, public_transports, nodes, cur_time) {

    /*
    ///////TODO: See if this can be made as a function
    var node_home_quarantined = false;
    if(INTERVENTION == HOME_QUARANTINE && node['compliant']){
        var house_members = homes[node['home']]['individuals'];
        for (var l = 0; l < house_members.length; l++){
            var time_since_symptoms = cur_time - nodes[house_members[l]]['time_since_infection'] - nodes[house_members[l]]['incubation_period'] - nodes[house_members[l]]['asymptomatic_period'];
            node_home_quarantined = node_home_quarantined ||
            (	(nodes[house_members[l]]['infection_status']!=SUSCEPTIBLE) &&
                (nodes[house_members[l]]['infection_status']!=EXPOSED) &&
                (time_since_symptoms > NUM_DAYS_TO_RECOG_SYMPTOMS*SIM_STEPS_PER_DAY) &&
                (time_since_symptoms <= (NUM_DAYS_TO_RECOG_SYMPTOMS+HOME_QUARANTINE_DAYS)*SIM_STEPS_PER_DAY)	);
        }
    }
    */
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
        //add the community lambda with a distance related scaling factor
        //console.log("In lambda incoming.")
        node['lambda_incoming'][3] = node['kappa_PT_incoming'] * node['dist_hw'] * public_transports[node['public_transport']]['lambda_PT'];
    }

    node['lambda'] = node['lambda_incoming'][0] + node['lambda_incoming'][1] + node['lambda_incoming'][2] + node['lambda_incoming'][3];

}

/*
function get_lambda_stats(time,node,lambda_stats_variable){
	var temp = [time,node,0,0,0,0];
	if(node['home']!=null) {temp[2]=homes[node['home']]['lambda_home'];}
	if(node['workplace']!=null ) {temp[3] = workplaces[node['workplace']]['lambda_workplace'];}
	if(node['community']!=null) {temp[4] = node['zeta_a']*node['funct_d_ck']*communities[node['community']]['lambda_community'];}
	temp[5] = temp[2]+temp[3]+temp[4]
	lambda_stats_variable.push(temp)
}
*/

compute.prototype.update_lambda_stats=function(node) {
    var sum_inv = 1 / math.sum(node['lambda_incoming']);
    var lambda_vector = [];
    for (var i = 0; i < node['lambda_incoming'].length; i++) {
        lambda_vector.push(node['lambda_incoming'][i] * sum_inv);
        LAMBDA_INFECTION_MEAN[i] = (LAMBDA_INFECTION_MEAN[i] * LAMBDA_INFECTION_STATS.length + lambda_vector[i]) / (LAMBDA_INFECTION_STATS.length + 1);
    }
    LAMBDA_INFECTION_STATS.push(lambda_vector)

}


let csvContent_alltogether = "data:text/csv;charset=utf-8,";

compute.prototype.update_all_kappa=function(nodes, homes, workplaces, communities, cur_time) {
    var current_time = cur_time;
    if (current_time < NUM_DAYS_BEFORE_INTERVENTIONS * SIM_STEPS_PER_DAY) {
        get_kappa_no_intervention(nodes, homes, workplaces, communities, current_time,this);
    } else {
        //current_time = current_time - NUM_DAYS_BEFORE_INTERVENTIONS*SIM_STEPS_PER_DAY;
        switch (INTERVENTION) {
            case CALIBRATION:
                get_kappa_CALIBRATION(nodes, homes, workplaces, communities, current_time,this);
                break;
            case NO_INTERVENTION:
                get_kappa_no_intervention(nodes, homes, workplaces, communities, current_time,this);
                break;
            case CASE_ISOLATION:
                get_kappa_case_isolation(nodes, homes, workplaces, communities, current_time,this);
                break;
            case HOME_QUARANTINE:
                get_kappa_home_quarantine(nodes, homes, workplaces, communities, current_time,this);
                break;
            case LOCKDOWN:
                get_kappa_lockdown(nodes, homes, workplaces, communities, current_time,this);
                break;
            case CASE_ISOLATION_AND_HOME_QUARANTINE:
                get_kappa_CI_HQ(nodes, homes, workplaces, communities, current_time,this);
                break;
            case CASE_ISOLATION_AND_HOME_QUARANTINE_SD_70_PLUS:
                get_kappa_CI_HQ_70P(nodes, homes, workplaces, communities, current_time,this);
                break;
            case LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI:
                get_kappa_LOCKDOWN_21_CI_HQ_SD_70_PLUS_21_CI(nodes, homes, workplaces, communities, current_time,this);
                break;
            case LOCKDOWN_21:
                get_kappa_LOCKDOWN_21(nodes, homes, workplaces, communities, current_time,this);
                break;
            case LD_21_CI_HQ_SD70_SC_21_SC_42:
                get_kappa_LD_21_CI_HQ_SD70_SC_21_SC_42(nodes, homes, workplaces, communities, current_time,this);
                break;
            case LD_21_CI_HQ_SD70_SC_21:
                get_kappa_LD_21_CI_HQ_SD70_SC_21(nodes, homes, workplaces, communities, current_time,this);
                break;
            case LD_21_CI_HQ_SD70_SC_OE_30:
                get_kappa_LD_21_CI_HQ_SD70_SC_OE_30(nodes, homes, workplaces, communities, current_time,this);
                break;
            default:
                break;
        }
    }
}


compute.prototype.run_simday=function(time_step, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
                    days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution) {

    console.log(time_step / SIM_STEPS_PER_DAY);

    //seeding strategies
    if (SEEDING_MODE == SEED_INFECTION_RATES && time_step < seed_array.length) {
        infection_seeding(nodes, seed_array[time_step], time_step);
    } else if (SEEDING_MODE == SEED_EXP_RATE) {
        this.infection_seeding_exp_rate(nodes, time_step);
    }

    for (var j = 0; j < NUM_PEOPLE; j++) {
        this.update_infection(nodes[j], time_step);
        //update_kappa(nodes[j], time_step);
        this.update_psi(nodes[j], time_step);
    }
    this.update_all_kappa(nodes, homes, workplaces, communities, time_step);
    for (var h = 0; h < NUM_HOMES; h++) {
        homes[h]['age_dependent_mixing'] = this.update_lambda_h(nodes, homes[h]);
    }
    for (var w = 0; w < NUM_SCHOOLS + NUM_WORKPLACES; w++) {
        workplaces[w]['age_dependent_mixing'] = this.update_lambda_w(nodes, workplaces[w]);
    }

    for (var c = 0; c < NUM_COMMUNITIES; c++) {
        communities[c]['lambda_community'] = this.update_lambda_c_local(nodes, communities[c]);
        ///console.log("lambda_community:",c,communities[c]['lambda_community'])
        var temp_stats = this.get_infected_community(nodes, communities[c]);

        //infection_status_community.push([]);
        let row = [time_step / SIM_STEPS_PER_DAY, c, temp_stats[0], temp_stats[1], temp_stats[2], temp_stats[3], temp_stats[4]].join(",");
        csvContent += row + "\r\n";
    }
    for (var pt = 0; pt < NUM_PUBLIC_TRANSPORT; pt++) {
        public_transports[pt]['lambda_PT'] = this.update_lambda_public_transport(nodes, public_transports[pt]);
    }


    this.update_lambda_c_global(communities, community_distance_matrix);


    for (var j = 0; j < NUM_PEOPLE; j++) {
        var lambda_current_stats = [];
        this.update_lambdas(nodes[j], homes, workplaces, communities, public_transports, nodes, time_step);
        //get_lambda_stats(i,j,lambda_current_stats);
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
        return time_step;
        // return [time_step, days_num_infected, days_num_exposed, days_num_hospitalised, days_num_critical, days_num_fatalities, days_num_recovered, days_num_affected, lambda_evolution];
    } else {
        return this.run_simday(time_step + 1, homes, workplaces, communities, public_transports, nodes, community_distance_matrix, seed_array,
            days_num_affected, days_num_critical, days_num_exposed, days_num_fatalities, days_num_hospitalised, days_num_infected, days_num_recovered, lambda_evolution);
    }


}
compute.prototype.euclidean=function(loc1, loc2) {
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
        //dist = Math.acos(dist);
        ///dist = dist * 180/Math.PI;
        //dist = dist * 60 * 1.1515;
        //if (unit=="K") { dist = dist * 1.609344 }
        //if (unit=="N") { dist = dist * 0.8684 }
        return dist;
    }
}
compute.prototype.update_infection=function(node, cur_time) {


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

    //console.log(1-Math.exp(-node['lambda']/SIM_STEPS_PER_DAY))
    ///TODO: Parametrise transition times
    if (node['infection_status'] == SUSCEPTIBLE && Math.random() < (1 - Math.exp(-node['lambda'] / SIM_STEPS_PER_DAY))) {
        node['infection_status'] = EXPOSED; //move to exposed state
        node['time_of_infection'] = cur_time;
        node['infective'] = 0;
        this.update_lambda_stats(node)
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


    node['lambda_h'] = this.update_individual_lambda_h(node);
    node['lambda_w'] = this.update_individual_lambda_w(node);
    node['lambda_c'] = this.update_individual_lambda_c(node);
    node['lambda_pt'] = this.update_individual_lambda_pt(node);
}

compute.prototype.infection_seeding=function(nodes, seed_count, curr_time) {
    var num_individuals_to_seed = seed_count;
    var num_seeded = 0;
    while (num_seeded < num_individuals_to_seed) {
        let individual_index = d3.randomInt(0, NUM_PEOPLE)();
        if (nodes[individual_index]['infection_status'] == SUSCEPTIBLE) {
            nodes[individual_index]['infection_status'] = EXPOSED;
            nodes[individual_index]['time_of_infection'] = curr_time; //*Math.random(); //TODO. Need to revisit.
            num_seeded++;
        }
    }
    console.log(curr_time / SIM_STEPS_PER_DAY, num_individuals_to_seed);
}