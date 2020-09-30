#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "agents_store.pb.h"
#include "models.h"
#include "cohorts.h"
#include "train_loader.h"
#include "intervention_primitives.h"

using namespace std;

namespace {

vector<cohort_space*> reset_coach_assignment(
    std::unordered_map<count_type, std::vector<cohort_space>>& cohorts, 
    bool isOneOff){

    vector<cohort_space*> ptr_cohort_vector;
    for (auto& it1: cohorts){
        for (auto& cohort: it1.second){
            if (cohort.is_one_off_cohort != isOneOff) {
                continue;
            }
            ptr_cohort_vector.push_back(&cohort);
        }
    }
    std::random_shuffle (ptr_cohort_vector.begin(), ptr_cohort_vector.end()); //What would be the random generator. Will it be the same permutation all the time?
    return ptr_cohort_vector;
}
bool custom_sort(pair<int, int> i, pair<int, int> j) {
    return i.second > j.second; // Reverse order.
}
void drop_cohorts_randomly(
    std::unordered_map<count_type, std::vector<cohort_space>>& cohorts,
    double taking_train_fraction) {
    if (taking_train_fraction > 1.0 || taking_train_fraction < 0.0) {
        cout << "ERROR taking_train_fraction out of bounds. Skipping drop of cohorts\n";
        return;
    }

    vector<pair<int, int>> vec;
    int total_elements_before = 0;
    int total_elements_after = 0;
    for (auto& it1: cohorts){
        total_elements_before += it1.second.size();
        for (size_t i = 0; i < it1.second.size(); i++) {
            vec.push_back(make_pair(it1.first, i));
        }
    }
    std::random_shuffle (vec.begin(), vec.end());
    int elements_to_keep = taking_train_fraction * vec.size();
    vec.resize(vec.size() - elements_to_keep);

    std::sort(vec.begin(), vec.end(), custom_sort);

    for (auto& i : vec) {
        cohorts[i.first].erase(cohorts[i.first].begin() + i.second);
    }

    for (auto& it1: cohorts){
        total_elements_after += it1.second.size();
    }
    cout << " \n\n Size before drop " << total_elements_before
          << " after drop " << total_elements_after
          << " dropped size " << vec.size();
}

}

int get_num_cohorts(const std::unordered_map<count_type, std::vector<cohort_space>>& cohorts){
    //This could be stored separately
    int num_cohorts = 0;
    for (auto& it1: cohorts){
        num_cohorts+=it1.second.size();
    }
    return num_cohorts;
}

void storeAgentsInfo(const std::vector<agent>& nodes) {
    if (GLOBAL.output_path.empty()) {
        std::cout << "Output path missing for store. ERROR." << std::endl;
        return;
    }

    cohorts::AgentStore agentStore;
    for (const auto& node: nodes) {
        auto* agentElemPtr = agentStore.add_agents();
        agentElemPtr->set_infective(node.infective);
        agentElemPtr->set_entered_symptomatic_state(node.entered_symptomatic_state);
        agentElemPtr->set_entered_hospitalised_state(node.entered_hospitalised_state);
        agentElemPtr->set_infection_status((int)node.infection_status);
        agentElemPtr->set_state_before_recovery((int)node.state_before_recovery);
        agentElemPtr->set_disease_label((int) node.disease_label);
        agentElemPtr->set_time_of_infection(node.time_of_infection);
        agentElemPtr->set_time_became_infective(node.time_became_infective);

        auto* test_status = agentElemPtr->mutable_test_status();
        test_status->set_tested_epoch(node.test_status.tested_epoch);
        test_status->set_tested_positive(node.test_status.tested_positive);
        test_status->set_contact_traced_epoch((int) node.test_status.contact_traced_epoch);
        test_status->set_test_requested(node.test_status.test_requested);
        test_status->set_state((int) node.test_status.state);
        test_status->set_triggered_contact_trace(node.test_status.triggered_contact_trace);
        test_status->set_node_test_trigger((int) node.test_status.node_test_trigger);
    }
    ofstream agentStoreFile;

    agentStoreFile.open(GLOBAL.output_path + "/agentStore.pbstore");
    if (!agentStoreFile.good()) {
        std::cout << "ERROR: Store file can't be opened" << std::endl;
        return;
    }
    agentStore.SerializeToOstream(&agentStoreFile);
    agentStoreFile.close();
}

bool loadAgentsInfo(std::vector<agent>& nodes) {
    cohorts::AgentStore agentStore;
    ifstream agentStoreFile;
    agentStoreFile.open(GLOBAL.input_base + "/agentStore.pbstore");
    if (!agentStoreFile.good()) {
        std::cout << "ERROR: Load file doesn't exist" << std::endl;
        return false;
    }
    agentStore.ParseFromIstream(&agentStoreFile);
    agentStoreFile.close();
    if (nodes.size() != agentStore.agents_size()) {
        cout << "ERROR: load agent info mismatch "
             << " nodes.size() " << nodes.size()
             << " proto size " << agentStore.agents_size()
             << std::endl;
        return false;
    }

    for (int i=0; i < agentStore.agents_size(); i++) {
        nodes[i].infective = agentStore.agents().at(i).infective();
        nodes[i].entered_symptomatic_state = agentStore.agents().at(i).entered_symptomatic_state();
        nodes[i].entered_hospitalised_state = agentStore.agents().at(i).entered_hospitalised_state();
        nodes[i].infection_status = static_cast<Progression>(agentStore.agents().at(i).infection_status());
        nodes[i].state_before_recovery = static_cast<Progression>(agentStore.agents().at(i).state_before_recovery());
        nodes[i].disease_label = static_cast<DiseaseLabel>(agentStore.agents().at(i).disease_label());
        nodes[i].time_of_infection = (agentStore.agents().at(i).time_of_infection());
        nodes[i].time_became_infective = (agentStore.agents().at(i).time_became_infective());

        nodes[i].test_status.tested_epoch = agentStore.agents().at(i).test_status().tested_epoch();
        nodes[i].test_status.tested_positive = agentStore.agents().at(i).test_status().tested_positive();
        nodes[i].test_status.contact_traced_epoch = agentStore.agents().at(i).test_status().contact_traced_epoch();
        nodes[i].test_status.test_requested = agentStore.agents().at(i).test_status().test_requested();
        nodes[i].test_status.state = static_cast<test_result>(agentStore.agents().at(i).test_status().state());
        nodes[i].test_status.triggered_contact_trace = agentStore.agents().at(i).test_status().triggered_contact_trace();
        nodes[i].test_status.node_test_trigger = static_cast<test_trigger>(agentStore.agents().at(i).test_status().node_test_trigger());
    }
    return true;
}

vector<tuple<int, bool, int, double>> get_coach_occupancy_per_line(
        const unordered_map<count_type, vector<train_coach>>& coachs) {

    std::vector<std::tuple<int, bool, int, double>> v;
    for (const auto& line: coachs) {
        int up_coach_count = 0;
        int down_coach_count = 0;
        int total_capacity = 0;
        map<int, int> up; // station, available_capacity map
        map<int, int> down; // station, available_capacity map
        unordered_map<int, int> per_coach; // coach, available_capacity map
        for (const auto& coach: line.second) {
            // Per coach
            per_coach[coach.coach_id] = 0;
            if (coach.isDown) {
                down_coach_count++;
            } else {
                up_coach_count++;
            }

            for (const auto& station : coach.capacity_at_station) {
                // Total
                total_capacity += station.second;
                per_coach[coach.coach_id] += station.second;
                // Per stations.
                if (coach.isDown) {
                    if (down.find(station.first) == down.end()) {
                        down[station.first] = 0;
                    }
                    down[station.first] += station.second;
                } else {
                    if (up.find(station.first) == up.end()) {
                        up[station.first] = 0;
                    }
                    up[station.first] += station.second;
                }
            }
        }

        cout << "\nLine:" << line.first
            << " Stations along line:" << up.size()
            << " total_coach:"<< line.second.size() << " " << per_coach.size()
            << " up:" << up_coach_count
            << " down:" << down_coach_count
            << " avg capacity: " << ((double) total_capacity) / ((double)(up.size()*per_coach.size()));
        cout << "\n\t Up";
        for (auto& u : up) {
            double station_capacity = ((double) u.second) / ((double)up_coach_count);
            v.emplace_back(make_tuple(line.first,false, u.first, station_capacity));
            cout  << "\n\t\t station:"<< u.first << " cap:"<< station_capacity;
        }
        cout << "\n\t Down";
        for (auto& d : down) {
            double station_capacity = ((double) d.second) / ((double)down_coach_count);
            v.emplace_back(make_tuple(line.first,true, d.first, station_capacity));
            cout  << "\n\t\t station:"<< d.first << " cap:"<< station_capacity;
        }
    }
    return v;
}

TrainLoader init_TrainLoader() {
    std::string overlap_file = GLOBAL.input_base + "overlap_time.json";
    std::string travel_file = GLOBAL.input_base + "travel_time.json";
    std::string route_file = GLOBAL.input_base + "train_route.json";

    TrainLoader train_loader(overlap_file, travel_file, route_file);
    return train_loader;
}

void merge_coaches(std::unordered_map<count_type, std::vector<train_coach>>& to,
    const std::unordered_map<count_type, std::vector<train_coach>>& from) {

    for (auto& f : from) {
        if (to.find(f.first) == to.end()) {
            to[f.first] = f.second;
            continue;
        }
        // Copy vectors from one to the other.
        to[f.first].insert(to[f.first].end(), f.second.begin(), f.second.end());
    }
}

void merge_cohorts(std::unordered_map<count_type, std::vector<cohort_space>>& to,
   const std::unordered_map<count_type, std::vector<cohort_space>>& from) {
       for (const auto& iter1: from) {
           auto jter1 = to.find(iter1.first);
           if (jter1 == to.end()) {
               to[iter1.first] = iter1.second;
               continue;
           }
           jter1->second.insert(jter1->second.end(), iter1.second.begin(), iter1.second.end());        
       }
}

unordered_map<count_type, vector<cohort_space>> make_oneoff_cohorts(
    vector<agent>& nodes, const TrainLoader& trains) {
    unordered_map<count_type, vector<cohort_space>> cohorts_at_station;
    if(!GLOBAL.ENABLE_COHORTS){
        return {};
    }
    int cohort_id = 0; // These ids will overlap with normal cohorts. This is fine since they won't share coaches.
    for (count_type i=0; i<nodes.size(); ++i){ 
        if(nodes[i].workplace_type != WorkplaceType::office 
            || !nodes[i].my_cohort.takes_train || !nodes[i].my_cohort.one_off_traveler){
            continue;
        }
        int source = nodes[i].my_cohort.source_station;
        int destination = nodes[i].my_cohort.destination_station;
        int key = source*100 + destination;
        if (cohorts_at_station.find(key) == cohorts_at_station.end()) {
            cohorts_at_station[key] = {};
        }
        cohort_space new_cohort(cohort_id++, source, destination, trains.GetJourneyMinutes(source, destination));
        new_cohort.internal_nodes.push_back(i);
        new_cohort.is_one_off_cohort = true;
        cohorts_at_station[key].push_back(new_cohort);
    }
    return cohorts_at_station;
}


unordered_map<count_type, vector<cohort_space>> make_cohorts(
    vector<agent>& nodes, count_type target_size, const TrainLoader& trains){
    unordered_map<count_type, vector<cohort_space>> cohorts_at_station;
    vector<cohort_space> empty_cohort;

    //TODO[v2]: consider the following values: double fraction_in_train, double crowding_factor,
    if(!GLOBAL.ENABLE_COHORTS){
        return {};
    }
    //unordered_map to map source, destination and the employees who travel between source, and destination
    unordered_map<int, unordered_map<int, vector<count_type>>> stations_and_people;
    unordered_map<int, vector<count_type>> at_source;

    //iterate across all employed individuals
    for (count_type i=0; i<nodes.size(); ++i){
        if(nodes[i].workplace_type != WorkplaceType::office 
            || !nodes[i].my_cohort.takes_train || nodes[i].my_cohort.one_off_traveler){
            continue;
        }
        int source = nodes[i].my_cohort.source_station;
        int destination = nodes[i].my_cohort.destination_station;

        //group individuals with same source and destination stations
        if (stations_and_people.find(source) == stations_and_people.end()) {
            //if new source: add both the source and destination
            stations_and_people[source] = at_source;
            vector<count_type> agent_vector = {i};
            stations_and_people[source][destination] = agent_vector;
        }
        else{
            //source exists
            if (stations_and_people[source].find(destination) == stations_and_people[source].end()) {
                vector<count_type> agent_vector = {i};
                stations_and_people[source][destination] = agent_vector;
            }
            else{
                stations_and_people[source][destination].push_back(i);
            }
        }
    }

    //create cohorts
    for (auto& it1: stations_and_people) {
        int source_index = it1.first;
        for (auto& it2: it1.second) {
            int cohort_count = 0;
            cohorts_at_station[(it1.first*100) + it2.first] = vector<cohort_space>();
            auto citer = it2.second.begin();
            cohort_space new_cohort(cohort_count, source_index, it2.first, trains.GetJourneyMinutes(source_index, it2.first));
            while(citer!= it2.second.end()) {
                new_cohort.internal_nodes.push_back(*citer);
                if (new_cohort.internal_nodes.size() == target_size || ((citer+1)==it2.second.end())) {
                    // storing cohort.
                    new_cohort.is_one_off_cohort = false;
                    cohorts_at_station[(it1.first*100) + it2.first].push_back(new_cohort);
                    new_cohort = cohort_space(++cohort_count, source_index, it2.first, trains.GetJourneyMinutes(source_index, it2.first));
                }
                citer++;
            }
        }
    }    
    drop_cohorts_randomly(cohorts_at_station, GLOBAL.taking_train_fraction);
    return cohorts_at_station;
}

void compute_scale_intra_cohorts(unordered_map<count_type, vector<cohort_space>>& cohorts, const vector<agent>& nodes){
    if(GLOBAL.ENABLE_COHORTS){
        for (auto& it1: cohorts) { //cohort_hash
            for (auto& cohort_iterator: it1.second) {
                //double sum_value = cohort_iterator.internal_nodes.size();
                if (cohort_iterator.internal_nodes.size() == 0){
                    cohort_iterator.scale = 0;
                }
                else cohort_iterator.scale = GLOBAL.BETA_COHORT; // cohort_iterator.commute_time; // sum_value; //scaling with commute time will in update_lambdas
            }
        }
    }
}


std::unordered_map<count_type, std::vector<train_coach>> make_train_coaches(
  std::unordered_map<count_type, std::vector<cohort_space>>& cohorts,
  const TrainLoader& trains, bool isAM, bool isOneOff) {

    std::unordered_map<count_type, std::vector<train_coach>> train_coaches;

    if(!GLOBAL.ENABLE_COHORTS){
        return train_coaches;
    }
    auto random_cohort_ptr = reset_coach_assignment(cohorts, isOneOff);
    int coach_counter = 0;


    int num_lines = 4; //need to be updated to actual number of lines. Need to add to GLOBAL?
    for(int line_count = 0; line_count<num_lines;++line_count){
        vector<train_coach> temp;
        train_coaches[line_count+1] = temp; //train linenumber starts from 1
    }
    int rejected_cohorts_threshold = 5; //Need to add to GLOBAL?
    const int capacity = GLOBAL.COACH_SEAT_CAPACITY * GLOBAL.crowding_factor;

    vector<int> rejected_up_cohorts(num_lines,0); //counter for rejected cohorts for each line. Might need to make it unordered map with trainline as key.
    vector<int> rejected_down_cohorts(num_lines,0);

    vector<train_coach> current_up_coaches; //current set of active coaches on each line
    vector<train_coach> current_down_coaches;
    for(int i=1; i<=num_lines;i++){
        auto temp_stations=trains.GetTrainLineStations(i,true);
        train_coach temp(coach_counter++, i,true,capacity, temp_stations);
        current_up_coaches.push_back(temp);
        temp_stations=trains.GetTrainLineStations(i,false);
        train_coach temp1(coach_counter++, i,false,capacity,temp_stations);
        current_down_coaches.push_back(temp1);
    }
    int back_index = random_cohort_ptr.size() -1;
    while(!random_cohort_ptr.empty()){
        // int random_index = uniform_count_type(0,random_cohort_ptr.size()-1); //confirm that uniform_count_type gives a value between o and size-1.
        back_index = (back_index < 0) ?  0 : back_index;
        auto current_cohort = random_cohort_ptr[back_index];// We will assume that this is an un-assigned cohort
        int current_src, current_dest;
        if (isAM) {
            current_src = current_cohort->source_station;
            current_dest = current_cohort->destination_station;
        } else {
            current_src = current_cohort->destination_station;
            current_dest = current_cohort->source_station;
        }
        bool cohort_assigned = true;
        auto current_cohort_journey_legs =  trains.GetJourneyLegs(current_src, current_dest);
        int cohort_size = current_cohort->internal_nodes.size();

        for (auto& journey: current_cohort_journey_legs){
            std::vector<int> stations;
            auto current_trainline = journey.trainLine;
            auto isDown = trains.GetJourneyLegStationsAndIsDown(current_trainline, journey.src, journey.dest,stations);
            for (count_type count = 0; count < stations.size()-1; ++count){
                auto station = stations[count];
                if(isDown){
                    if(current_down_coaches[current_trainline-1].capacity_at_station[station] <= cohort_size){
                        cohort_assigned = false;
                        rejected_down_cohorts[current_trainline-1]++;
                        break;
                    }
                } else{
                    if(current_up_coaches[current_trainline-1].capacity_at_station[station] <= cohort_size){
                        cohort_assigned = false;
                        rejected_up_cohorts[current_trainline-1]++;
                        break;
                    }
                }
            }
        }

        if(cohort_assigned){
            for (auto& journey: current_cohort_journey_legs){
                std::vector<int> stations;
                auto current_trainline = journey.trainLine;
                auto isDown = trains.GetJourneyLegStationsAndIsDown(current_trainline, journey.src, journey.dest,stations);
                for (count_type count = 0; count < stations.size()-1; ++count){ // Exclude last station as they will deboard here.
                    auto station = stations[count];
                    if(isDown){
                        current_down_coaches[current_trainline-1].capacity_at_station[station] -= cohort_size;
                    } else{
                        current_up_coaches[current_trainline-1].capacity_at_station[station] -= cohort_size;
                    }
                }
                if(isDown){
                    current_down_coaches[current_trainline-1].cohorts.push_back(current_cohort);
                }else{
                    current_up_coaches[current_trainline-1].cohorts.push_back(current_cohort);
                }
            }
            random_cohort_ptr.erase(random_cohort_ptr.begin()+back_index);
            back_index--;
        }else{
            back_index--;
            for (int line_count = 0; line_count<num_lines;++line_count){
                if(rejected_down_cohorts[line_count] > rejected_cohorts_threshold){
                    train_coaches[line_count+1].push_back(current_down_coaches[line_count]);
                    current_down_coaches[line_count].reset(coach_counter++, capacity);
                    rejected_down_cohorts[line_count] = 0;
                    back_index = random_cohort_ptr.size() - 1;
                }
                if(rejected_up_cohorts[line_count] > rejected_cohorts_threshold){
                    train_coaches[line_count+1].push_back(current_up_coaches[line_count]);
                    current_up_coaches[line_count].reset(coach_counter++, capacity);
                    rejected_up_cohorts[line_count] = 0;
                    back_index = random_cohort_ptr.size() - 1;
                }
            }
        }
    }
    for (int line_count = 0; line_count<num_lines;++line_count){
        train_coaches[line_count+1].push_back(current_down_coaches[line_count]);
        train_coaches[line_count+1].push_back(current_up_coaches[line_count]);
    }
#ifdef DEBUG
    get_coach_occupancy_per_line(train_coaches);
#endif
    return train_coaches;
}

// creation of inter_cohorts to use unordered_maps
void assign_inter_cohort(unordered_map<count_type, vector<train_coach>>& coachs,
                         const vector<agent>& nodes){
    if(!GLOBAL.ENABLE_COHORTS){
        return;
    }
}

//TODO[v2]: Implement a method to assign cohorts to trains (inter-cohort modelling)

void update_kappas_cohorts(unordered_map<count_type, vector<cohort_space>>& cohorts , vector<agent>& nodes, int cur_time){
 //intervention modeling for cohorts.
    if(GLOBAL.ISOLATE_COHORTS && GLOBAL.TRAINS_RUNNING){
        for (auto& it1: cohorts) { //cohort_hash (src*100) + dst
            for (auto& cohort_it: it1.second) {
                cohort_it.quarantined = false; //reset quarantine to false.
                for (auto& j: cohort_it.internal_nodes){
                    // double time_since_symptomatic = cur_time
                    //     - (nodes[j].time_of_infection
                    //         + nodes[j].incubation_period
                    //         + nodes[j].asymptomatic_period);
                    // double time_since_hospitalised = time_since_symptomatic -nodes[j].symptomatic_period;
                    nodes[j].my_cohort.quarantined = false; //Reset qquarantined flag. Will be set in the below loop if the cohort needs to be quarantined.
                    const bool symptomatic_severe_compliant = ((nodes[j].entered_symptomatic_state) &&
                        (nodes[j].severity_index <= GLOBAL.COHORT_SEVERITY_FRACTION) &&
                        nodes[j].compliant); //Node is symptomatic, has severe symptoms and is compliant
                    const bool tested_positive = nodes[j].test_status.tested_positive; // node tested positive at some point.
                    const bool hospitalised = nodes[j].entered_hospitalised_state; //node entered hospital state.

                    double trigger_time = GLOBAL.NUM_TIMESTEPS; //time at which this individual triggers isolation. Starting with a large value.

                    if(symptomatic_severe_compliant){
                        trigger_time = min(trigger_time,(nodes[j].time_of_infection
                            + nodes[j].incubation_period
                            + nodes[j].asymptomatic_period));
                    }
                    if(tested_positive){
                        trigger_time = min(trigger_time,(double)nodes[j].test_status.contact_traced_epoch);
                    }
                    if(hospitalised){
                        trigger_time = min(trigger_time,((nodes[j].time_of_infection
                            + nodes[j].incubation_period
                            + nodes[j].asymptomatic_period
                            + nodes[j].symptomatic_period)));
                    }


                    if((symptomatic_severe_compliant || tested_positive || hospitalised) && (cur_time-trigger_time <= HOME_QUARANTINE_DAYS*GLOBAL.SIM_STEPS_PER_DAY)){
                            cohort_it.quarantined = true;
                            break;
                    }
                }
                if(cohort_it.quarantined){
                    for (auto& j: cohort_it.internal_nodes){
                        nodes[j].my_cohort.quarantined = true;
                        modify_kappa_case_isolate_node(nodes[j]); //TODO: Compliance per node on isolating themselves. Might need a separate cohort_compliance variable.
                    }
                }
            }
        }
    }
}
