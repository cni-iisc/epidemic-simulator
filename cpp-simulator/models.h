//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef MODELS_H_
#define MODELS_H_
#include <vector>
#include <random>
#include <tuple>
#include <cmath>
#include <string>
#include <algorithm>

enum class Intervention {
   no_intervention = 0,
   case_isolation = 1,
   home_quarantine = 2,
   lockdown = 3,
   case_isolation_and_home_quarantine = 4,
   case_isolation_and_home_quarantine_sd_65_plus = 5,
   lockdown_fper_ci_hq_sd_65_plus_sper_ci = 6,
   lockdown_fper = 7,
   ld_fper_ci_hq_sd65_sc_sper_sc_tper = 8,
   ld_fper_ci_hq_sd65_sc_sper = 9,
   ld_fper_ci_hq_sd65_sc_oe_sper = 10,
   intv_fper_intv_sper_intv_tper = 11,
   intv_NYC=12,
   intv_Mum=13,
   intv_nbr_containment=14,
   intv_ward_containment=15,
   intv_file_read=16,
   intv_Mum_cyclic=17
};

enum class Cycle_Type {
  home = 0,
  individual = 1
};

struct location{
  double lat, lon; //latitude and longitude, in degrees
};

template<typename T>
using matrix = std::vector< std::vector<T> >;

//Type for storing counts
using count_type = unsigned long;
inline count_type stoct(const std::string& str){
  return std::stoul(str);
}

// Random number gnerators
#ifdef MERSENNE_TWISTER
extern std::mt19937_64 GENERATOR;
#else
extern std::default_random_engine GENERATOR;
#endif
void SEED_RNG();
void SEED_RNG_PROVIDED_SEED(count_type seed);

void SEED_RNG_GRAPH();
void SEED_RNG_GRAPH_PROVIDED_SEED(count_type seed);

inline double gamma(double shape, double scale){
  return std::gamma_distribution<double>(shape, scale)(GENERATOR);
}

inline bool bernoulli(double p){
  return std::bernoulli_distribution(p)(GENERATOR);
}

inline double uniform_real(double left, double right){
  return std::uniform_real_distribution<double>(left, right)(GENERATOR);
}

inline count_type uniform_count_type(double left, double right){
  return std::uniform_int_distribution<count_type>(left, right)(GENERATOR);
}


// Random number gnerators for random networks
#ifdef MERSENNE_TWISTER
extern std::mt19937_64 GENERATOR_NETWORK;
#else
extern std::default_random_engine GENERATOR_NETWORK;
#endif
void SEED_RNG();
void SEED_RNG_PROVIDED_SEED(count_type seed);

inline void randomly_shuffle(std::vector<int>& a){
  std::shuffle(a.begin(), a.end(), GENERATOR_NETWORK);  // Need to add a specific generator.
}

inline bool bernoulli_network(double p){
  return std::bernoulli_distribution(p)(GENERATOR_NETWORK);
}

inline double uniform_real_network(double left, double right){
  return std::uniform_real_distribution<double>(left, right)(GENERATOR_NETWORK);
}

inline double uniform_count_type_network(double left, double right){
  return std::uniform_int_distribution<count_type>(left, right)(GENERATOR_NETWORK);
}

// Global parameters
//age related transition probabilities, symptomatic to hospitalised to critical to fatality.
const double STATE_TRAN[][3] =
  {
   {0.0010000,   0.0500000,   0.4000000},
   {0.0030000,   0.0500000,   0.4000000},
   {0.0120000,   0.0500000,   0.5000000},
   {0.0320000,   0.0500000,   0.5000000},
   {0.0490000,   0.0630000,   0.5000000},
   {0.1020000,   0.1220000,   0.5000000},
   {0.1660000,   0.2740000,   0.5000000},
   {0.2430000,   0.4320000,   0.5000000},
   {0.2730000,   0.7090000,   0.5000000}
  };

struct testing_probability{
  count_type num_days = 0; //number of days for which this a protocol is active.
  double prob_test_index_symptomatic = 0;
  double prob_test_index_hospitalised = 0;

  double prob_test_household_positive_symptomatic = 0; // network_indexcase_contact
  double prob_test_household_hospitalised_symptomatic = 0;
  double prob_test_household_symptomatic_symptomatic = 0;
  double prob_test_household_positive_asymptomatic = 0;
  double prob_test_household_hospitalised_asymptomatic = 0;
  double prob_test_household_symptomatic_asymptomatic = 0;

  double prob_test_workplace_positive_symptomatic = 0;
  double prob_test_workplace_hospitalised_symptomatic = 0;
  double prob_test_workplace_symptomatic_symptomatic = 0;  
  double prob_test_workplace_positive_asymptomatic = 0;
  double prob_test_workplace_hospitalised_asymptomatic = 0;
  double prob_test_workplace_symptomatic_asymptomatic = 0;
  
  double prob_test_random_community_positive_symptomatic = 0;
  double prob_test_random_community_hospitalised_symptomatic = 0;
  double prob_test_random_community_symptomatic_symptomatic = 0;
  double prob_test_random_community_positive_asymptomatic = 0;
  double prob_test_random_community_hospitalised_asymptomatic = 0;
  double prob_test_random_community_symptomatic_asymptomatic = 0;

  double prob_test_neighbourhood_positive_symptomatic = 0;
  double prob_test_neighbourhood_hospitalised_symptomatic = 0;
  double prob_test_neighbourhood_symptomatic_symptomatic = 0;
  double prob_test_neighbourhood_positive_asymptomatic = 0;
  double prob_test_neighbourhood_hospitalised_asymptomatic = 0;
  double prob_test_neighbourhood_symptomatic_asymptomatic = 0;
 
  double prob_test_school_positive_symptomatic = 0;
  double prob_test_school_hospitalised_symptomatic = 0;
  double prob_test_school_symptomatic_symptomatic = 0;
  double prob_test_school_positive_asymptomatic = 0;
  double prob_test_school_hospitalised_asymptomatic = 0;
  double prob_test_school_symptomatic_asymptomatic = 0;
 
  double prob_retest_recovered = 0;
  
  double prob_contact_trace_household_symptomatic = 0;
  double prob_contact_trace_project_symptomatic = 0;
  double prob_contact_trace_random_community_symptomatic = 0;
  double prob_contact_trace_neighbourhood_symptomatic = 0;
  double prob_contact_trace_class_symptomatic = 0;

  double prob_contact_trace_household_hospitalised = 0;
  double prob_contact_trace_project_hospitalised = 0;
  double prob_contact_trace_random_community_hospitalised = 0;
  double prob_contact_trace_neighbourhood_hospitalised = 0;
  double prob_contact_trace_class_hospitalised = 0;

  double prob_contact_trace_household_positive = 0;
  double prob_contact_trace_project_positive = 0;
  double prob_contact_trace_random_community_positive = 0;
  double prob_contact_trace_neighbourhood_positive = 0;
  double prob_contact_trace_class_positive = 0;
};

  
enum class Testing_Protocol{
  no_testing,
  test_household,
  testing_protocol_file_read
};

struct svd {
  matrix<double> u, vT;
  std::vector<double> sigma;
};

struct kappa_values{
double kappa_H;
double kappa_H_incoming;
double kappa_W;
double kappa_W_incoming;
double kappa_C;
double kappa_C_incoming;
};

struct intervention_params {
  count_type num_days = 0;
  double compliance = 0.9;
  double compliance_hd = 0.9;
  bool case_isolation = false;
  bool home_quarantine = false;
  bool lockdown = false;
  bool social_dist_elderly = false; 
  bool school_closed = false;
  bool workplace_odd_even = false;
  double SC_factor = 0;
  double community_factor = 1;
  bool neighbourhood_containment = false;
  bool ward_containment = false;
  bool trains_active = false;
  double fraction_forced_to_take_train = 1;
  kappa_values lockdown_kappas_compliant;
  kappa_values lockdown_kappas_non_compliant;

  intervention_params(){
    lockdown_kappas_compliant.kappa_H = 2.0;
    lockdown_kappas_compliant.kappa_H_incoming = 1.0;
    lockdown_kappas_compliant.kappa_W = 0.25;
    lockdown_kappas_compliant.kappa_W_incoming = 0.25;
    lockdown_kappas_compliant.kappa_C = 0.25;
    lockdown_kappas_compliant.kappa_C_incoming = 0.25;

    lockdown_kappas_non_compliant.kappa_H = 1.25;
    lockdown_kappas_non_compliant.kappa_H_incoming = 1.0;
    lockdown_kappas_non_compliant.kappa_W = 0.25;
    lockdown_kappas_non_compliant.kappa_W_incoming = 0.25;
    lockdown_kappas_non_compliant.kappa_C = 1;
    lockdown_kappas_non_compliant.kappa_C_incoming = 1;
  }
  intervention_params& set_case_isolation(bool c){
	this->case_isolation = c;
	return *this;
  }
  intervention_params& set_home_quarantine(bool c){
	this->home_quarantine = c;
	return *this;
  }
  intervention_params& set_lockdown(bool c){
	this->lockdown = c;
	return *this;
  }
  intervention_params& set_social_dist_elderly(bool c){
	this->social_dist_elderly = c;
	return *this;
  }
  intervention_params& set_school_closed(bool c){
	this->school_closed = c;
	return *this;
  }
  intervention_params& set_workplace_odd_even(bool c){
	this->workplace_odd_even = c;
	return *this;
  }
  intervention_params& set_SC_factor(double c){
	this->SC_factor = c;
	return *this;
  }
  intervention_params& set_community_factor(double c){
	this->community_factor = c;
	return *this;
  }
};


//These are parameters associated with the disease progression
const double NUM_DAYS_TO_RECOG_SYMPTOMS = 1;
const bool SEED_INFECTION_FROM_FILE = false;
const double SELF_ISOLATION_DAYS = 7;
const double HOME_QUARANTINE_DAYS = 14;


// Global parameters
//
// The default values are as in the js simulator.  These are changed
// when the input files are read.
struct global_params{
  count_type RNG_SEED;
  count_type RNG_SEED_NETWORK;
  double COMPLIANCE_PROBABILITY = 1;
  double HD_COMPLIANCE_PROBABILITY = 1;
  count_type num_homes = 25000;
  count_type num_workplaces = 5000;
  count_type num_schools = 0;
  count_type num_communities = 198;

  count_type num_people = 100000;

  count_type NUM_DAYS = 120; //Number of days. Simulation duration
  const count_type SIM_STEPS_PER_DAY = 4; //Number of simulation steps per day.
  count_type NUM_TIMESTEPS = NUM_DAYS*SIM_STEPS_PER_DAY; //
  double INIT_FRAC_INFECTED = 0.0001; // Initial number of people infected

  double MEAN_INCUBATION_PERIOD = 4.50;
  double MEAN_ASYMPTOMATIC_PERIOD = 0.5;
  double MEAN_SYMPTOMATIC_PERIOD = 5;
  double MEAN_HOSPITAL_REGULAR_PERIOD = 8;
  double MEAN_HOSPITAL_CRITICAL_PERIOD = 8;

  //Distance kernel parameters.
  //
  //These correspond to ones from Bangalore. Actual parameters for any
  //given city are given at input.
  double F_KERNEL_A = 10.751;
  double F_KERNEL_B = 5.384;
	
  
  const double INCUBATION_PERIOD_SHAPE = 2.0; //Fixing this back to 2.0. To change incubation period, change incubation scale.
  double INCUBATION_PERIOD_SCALE = MEAN_INCUBATION_PERIOD*SIM_STEPS_PER_DAY / INCUBATION_PERIOD_SHAPE;// 2.29 days

  //Gamma with mean 1 and shape 0.25, as per Imperial College 16 March Report
  double INFECTIOUSNESS_SHAPE = 0.25;
  double INFECTIOUSNESS_SCALE = 4;  

  double SEVERITY_RATE = 0.5; //value used in sim.js

  double ASYMPTOMATIC_PERIOD = MEAN_ASYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY;
  // half a day
  double SYMPTOMATIC_PERIOD = MEAN_SYMPTOMATIC_PERIOD*SIM_STEPS_PER_DAY;
  // 5 days
  double HOSPITAL_REGULAR_PERIOD = MEAN_HOSPITAL_REGULAR_PERIOD*SIM_STEPS_PER_DAY;
  double HOSPITAL_CRITICAL_PERIOD = MEAN_HOSPITAL_CRITICAL_PERIOD*SIM_STEPS_PER_DAY;
  double SYMPTOMATIC_FRACTION = 0.67;

  Intervention INTERVENTION = Intervention::no_intervention;

  // Beta values
  double BETA_H = 0.47 *1.0; //Thailand data
  double BETA_W = 0.47 *2; //Thailand data
  double BETA_S = 0.94 *2; //Thailand data
  double BETA_C = 0.097*4.85; // Thailand data. Product = 0.47
  double BETA_PROJECT = 0;
  double BETA_CLASS = 0;
  double BETA_RANDOM_COMMUNITY = 0;
  double BETA_NBR_CELLS = 0;

  double ALPHA = 0.8;
  //exponent of number of people in a household while normalising
  //infection rate in a household.

  //Transport
  double BETA_TRAVEL = 10.0;// Validate against data
  double P_TRAIN = 0.9; // Probability with which an agent has to travel
  double FRACTION_FORCED_TO_TAKE_TRAIN = 1.0;
  // What fraction of people, among those who are attending work and take the
  // train in usual circumstances, are forced (in the absence of other
  // employer-provided means, for example) to take the train.  Only relevant
  // when TRAINS_RUNNING is true.

  bool TRAINS_RUNNING = false;

  //Multiplicative fatcor for infection rates in high density areas
  double HD_AREA_FACTOR = 2.0;

  double HD_AREA_EXPONENT = 0.3;

  //Lockdown periods
  double FIRST_PERIOD = 21;
  double SECOND_PERIOD = 21;
  double THIRD_PERIOD = 42;
  double OE_SECOND_PERIOD = 30;


  //Cyclic strategy
  bool CYCLIC_POLICY_ENABLED = false;
  //Are cycles assigned to individuals or homes?
  Cycle_Type CYCLIC_POLICY_TYPE = Cycle_Type::individual;
  count_type CYCLIC_POLICY_START_DAY = 0;
  count_type NUMBER_OF_CYCLIC_CLASSES = 3;
  //How many days does the individual work for in a single phase of the cycle?
  count_type PERIOD_OF_ATTENDANCE_CYCLE = 5;

  //Community lockdown threshold.
  //
  // Community is fully locked down if the number of hospitalized individuals
  //crosses this fraction
  double COMMUNITY_LOCK_THRESHOLD = 1E-3; //0.1%
  double LOCKED_COMMUNITY_LEAKAGE = 1.0;

  // Lockdown thresholds for neighborhood cells
  double NEIGHBORHOOD_LOCK_THRESHOLD = 1E-3; //0.1%
  double LOCKED_NEIGHBORHOOD_LEAKAGE = 1.0;
  bool ENABLE_NEIGHBORHOOD_SOFT_CONTAINMENT = false;

  count_type WARD_CONTAINMENT_THRESHOLD = 1; // threshold of hospitalised individuals in ward, beyond which the ward is quarantined.
  //Switches
  //If this is false, the file quarantinedPopulation.json is needed
  bool USE_SAME_INFECTION_PROB_FOR_ALL_WARDS = true;

  //If this is true, then the initial seeding is for all individuals,
  //not just those residing in non-high-density areas
  bool SEED_HD_AREA_POPULATION = false;

  //If this is true, then only those who do not have to use public
  //transport (i.e, with has_no_travel set to false) are initially
  //seeded
  bool SEED_ONLY_NON_COMMUTER = false;

  //If this is true, only a fixed number of initial infections is
  //seeded
  bool SEED_FIXED_NUMBER = false;
  count_type INIT_FIXED_NUMBER_INFECTED = 0;

  //Whether to ignore the attendance file
  bool IGNORE_ATTENDANCE_FILE = true;
  count_type NUMBER_OF_OFFICE_TYPES = 6; //Number of office types.
  double ATTENDANCE_LEAKAGE = 0.25; // Assume leakage in attendance.

  //Input and output
  std::string input_base;
  std::string attendance_filename;

  //Status
  count_type INIT_ACTUALLY_INFECTED = 0;

  //Calibration
  double CALIBRATION_DELAY = 22; //Assuming Simulator starts on March 1
  double DAYS_BEFORE_LOCKDOWN = 24; //March 1 - March 24
  double NUM_DAYS_BEFORE_INTERVENTIONS = CALIBRATION_DELAY + DAYS_BEFORE_LOCKDOWN;

  bool MASK_ACTIVE = false;
  double MASK_FACTOR = 0.8;
  double MASK_START_DATE = 0;//40+
  
  //Age stratification
  count_type NUM_AGE_GROUPS = 16;
  double SIGNIFICANT_EIGEN_VALUES = 3;
  bool USE_AGE_DEPENDENT_MIXING = false;

  //Neighbourhood containment. City limits in lat,lon
  location city_SW, city_NE;
  double NBR_CELL_SIZE = 1; //in km
  bool ENABLE_CONTAINMENT = false;
  bool ENABLE_NBR_CELLS = false;

  std::string intervention_filename = "intervention_params.json";

  double MIN_PROJECT_SIZE = 3; //Min and Max number of members in a project.
  double MAX_PROJECT_SIZE = 10;
  double MIN_CLASS_AGE = 5;
  double MAX_CLASS_AGE = 19;
  double MIN_RANDOM_COMMUNITY_SIZE = 2; //Min and Max number of households in a random community.
  double MAX_RANDOM_COMMUNITY_SIZE = 5;
  
  bool ENABLE_TESTING = false;
  double TEST_FALSE_NEGATIVE = 0; //Probability of a true positive person tests negative
  double TEST_FALSE_POSITIVE = 0; //Probability of a true negative person tests positive
  Testing_Protocol TESTING_PROTOCOL=Testing_Protocol::test_household;
  double TIME_TO_TEST_POSITIVE = 3;
  int MINIMUM_TEST_INTERVAL = 7; //Minumum duration between two consecutive tests
  std::string testing_protocol_filename = "testing_protocol.json";

};
extern global_params GLOBAL;

// return a random compliance based on GLOBAL.compliance_probability
inline bool compliance(){
  return bernoulli(GLOBAL.COMPLIANCE_PROBABILITY);
}

inline double get_non_compliance_metric(){
  return uniform_real(0,1);
}

//Age groups (5-years)

inline count_type get_age_group(int age){
  count_type age_group = age/5;
  return std::min(age_group, GLOBAL.NUM_AGE_GROUPS - 1);
}

// Age index for STATE_TRAN matrix
int get_age_index(int age);
double zeta(int age);
double f_kernel(double dist);


// End of global parameters

struct grid_cell{
  count_type cell_x = 0;
  count_type cell_y = 0; //latitude and longitude, in degrees
};

//Distance between two locations given by their latitude and longitude, in degrees
double earth_distance(location a, location b);

enum class Progression {
   susceptible = 0,
   exposed,
   infective,
   symptomatic,
   recovered,
   hospitalised,
   critical,
   dead
};

enum class DiseaseLabel{
   asymptomatic = 0, //neither contact traced nor tested positive
   primary_contact, //CCC1
   mild_symptomatic_tested, //CCC2
   moderate_symptomatic_tested, //DCHC
   severe_symptomatic_tested, //DCH
   icu, //ICU
   recovered,
   dead
};

enum class WorkplaceType{
   home = 0,
   office = 1,
   school = 2
};

enum class OfficeType{
   other = 0,
   sez = 1,
   government = 2,
   it = 3,
   construction = 4,
   hospital = 5
};

//Default workplace value for homebound individuals.
const int WORKPLACE_HOME = -1;

struct lambda_incoming_data {
  double home = 0;
  double work = 0;
  double community = 0;
  double travel = 0;
  double project = 0;
  double random_community = 0;
  double nbr_cell = 0;

  void set_zero(){
	home = 0;
	work = 0;
	community = 0;
	travel = 0;
	project = 0;
  	random_community = 0;
	nbr_cell = 0;
  }

  inline double sum() const {
	return home + work + community + travel + project + random_community + nbr_cell;
  }

  inline lambda_incoming_data operator/(long double d) const {
	lambda_incoming_data temp(*this);
	temp /= d;
	return temp;
  }

  inline lambda_incoming_data operator*(long double d) const {
	lambda_incoming_data temp(*this);
	temp *= d;
	return temp;
  }

  inline lambda_incoming_data operator-(const lambda_incoming_data& rhs) const {
	lambda_incoming_data temp(*this);
	temp -= rhs;
	return temp;
  }

  inline lambda_incoming_data operator+(const lambda_incoming_data& rhs) const {
	lambda_incoming_data temp(*this);
	temp += rhs;
	return temp;
  }

  inline lambda_incoming_data& operator/=(long double d){
	home /= d;
	work /= d;
	community /= d;
	travel /= d;
	project /= d;
	random_community /= d;
	nbr_cell /= d;
	return *this;
  }

  inline lambda_incoming_data& operator*=(long double d){
	home *= d;
	work *= d;
	community *= d;
	travel *= d;
	project *= d;
	random_community *= d;
	nbr_cell *= d;
	return *this;
  }

  inline lambda_incoming_data& operator+=(const lambda_incoming_data& rhs){
	home += rhs.home;
	work += rhs.work;
	community += rhs.community;
	travel += rhs.travel;
	project += rhs.project;
	random_community += rhs.random_community;
	nbr_cell += rhs.nbr_cell;
	return *this;
  }

  inline lambda_incoming_data& operator-=(const lambda_incoming_data& rhs){
	home -= rhs.home;
	work -= rhs.work;
	community -= rhs.community;
	travel -= rhs.travel;
	project -= rhs.project;
	random_community -= rhs.random_community;
	nbr_cell -= rhs.nbr_cell;
	return *this;
  }

  inline void mean_update(const lambda_incoming_data& update, count_type num){
	home += (update.home - home)/num;
	work += (update.work - work)/num;
	community += (update.community - community)/num;
	travel += (update.travel - travel)/num;
	project += (update.project -  project)/num;
	random_community += (update.random_community - random_community)/num;
	nbr_cell += (update.nbr_cell - nbr_cell)/num;
  }
};

enum class test_result{
  not_yet_tested,
  positive,
  negative,
};

enum class test_trigger{
  not_yet_requested,
  symptomatic,
  hospitalised,
  contact_traced,
  re_test
};

struct test_struct{
  int tested_epoch = -28; // This is reset in init_nodes
  bool tested_positive = false; // To indicate if the individual is tested positive at sometime in the past
  count_type contact_traced_epoch = 0;
  bool test_requested = false;
  test_result state = test_result::not_yet_tested;
  bool triggered_contact_trace = false;
  test_trigger node_test_trigger=test_trigger::not_yet_requested;
};

struct agent{
  location loc;
  int age;
  int age_group; //For later feature update: for age dependent mixing
  int age_index; //For the STATE_TRAN matrix
  double zeta_a = 1;
  double infectiousness;
  //a.k.a rho
  double severity;
  //a.k.a S_k, is 0 or
  int home; //index of household
  int workplace;

  int community;
  double time_of_infection = 0;
  // time_of_infection is initialized to zero before seeding

  Progression infection_status = Progression::susceptible;
  bool entered_symptomatic_state = false;
  bool entered_hospitalised_state = false;

  // for recovered nodes, what was the last stage before recovery?
  Progression state_before_recovery = Progression::recovered;

  bool infective = false;
  count_type time_became_infective = 0;

  double lambda_h = 0;
  //individuals contribution to his home cluster
  double lambda_w = 0;
  //individuals contribution to his workplace cluster
  double lambda_c = 0;
  //individuals contribution to his community
  double lambda_nbr_cell = 0;
  //individuals contribution to neighbourhood cell
  double lambda = 0;

  double kappa_T = 1;
  double psi_T = 0;
  double funct_d_ck;

  WorkplaceType workplace_type;
  //one of school, office, or home
  OfficeType office_type = OfficeType::other;
  int workplace_subnetwork = 0;
  int community_subnetwork = 0;

  lambda_incoming_data lambda_incoming;
  //infectiousness from home, workplace, community, travel as seen by
  //individual

  //Neighborhood cell containment
  double neighborhood_access_factor = 1.0;
  //access_factor for the neighborhood cell in which this node lives
  //set to 1 in case neighborhood cell is not enabled.

  bool compliant = true;
  
  double kappa_H = 1;
  double kappa_W = 1;
  double kappa_C = 1;

  double incubation_period;
  double asymptomatic_period;
  double symptomatic_period;
  

  double hospital_regular_period;
  double hospital_critical_period;

  double kappa_H_incoming = 1;
  double kappa_W_incoming = 1;
  double kappa_C_incoming = 1;
  bool quarantined = false;

  //Cyclic strategy class.
  //
  //If a cyclic workplace strategy is being followed, then every agent will get
  //a class, which will determine the periods in which it goes to work.
  count_type cyclic_strategy_class = 0;

  //Transporation
  bool has_to_travel = false; //does the agent take a train to go to
							  //work?
  bool forced_to_take_train = true;
  //Will the agent be forced to take the train today, as employer did not provide transit?
  
  double commute_distance = 0; //in km

  bool hd_area_resident = false;
  //Multiplication factor for high population density areas, such as slums
  double hd_area_factor = 1.0;
  double hd_area_exponent = 0;
  //only used if in the input file, some individuals are assigned to
  //slums or other high population density areas

  //Currently attending office or not
  bool attending = true;
  
  agent(){}
  // Is the agent curently traveling?
  inline bool travels() const {
	return forced_to_take_train
	  && has_to_travel && attending
	  && !((quarantined && compliant)
		   || infection_status == Progression::hospitalised
		   || infection_status == Progression::critical
		   || infection_status == Progression::dead);
  }

  //attendance probability at given time, for the agent
  double get_attendance_probability(count_type time) const;
  test_struct test_status;
  DiseaseLabel disease_label = DiseaseLabel::asymptomatic;
};


struct random_community{
  double lambda_random_community;
  count_type community;
  std::vector<int> households;
  double scale = 0;
};  


struct house{
  location loc;
  grid_cell neighbourhood;
  double lambda_home = 0;
  std::vector<int> individuals; //list of indices of individuals
  double Q_h = 1;
  count_type community; // ward index
  random_community random_households;  //to specify random community network 

  double lambda_random_community_outgoing;

  //Cyclic strategy class.
  //
  //If a cyclic workplace strategy is being followed, then every home will get a
  //class, which will determine the periods in which individuals in it go to
  //work, when CYCLIC_POLICY_TYPE is Count_Type::home.
  count_type cyclic_strategy_class = 0;

  double scale = 0;
  bool compliant;
  double non_compliance_metric = 0; //0 - compliant, 1 - non-compliant
  bool quarantined = false;
  double age_independent_mixing;
  std::vector<double> age_dependent_mixing;

  //Neighborhood cell containment
  double neighborhood_access_factor = 1.0;
  //access_factor for the neighborhood cell in which this house is located. Set
  //to 1 in case neighborhood cell is not enabled.

  //age_dependent_mixing not added yet, since it is unused
  house(){}
  house(double latitude, double longitude, bool compliance):
	loc{latitude, longitude}, compliant(compliance) {}

  void set(double latitude, double longitude, bool compliance, double non_compl_metric){
    this->loc = {latitude, longitude};
    this->compliant = compliance;
    this->non_compliance_metric = non_compl_metric;
	if(GLOBAL.ENABLE_NBR_CELLS){
	  set_nbr_cell();  //Requires location to be set
	}
  }

  void set_nbr_cell();
};


struct project{
  int workplace;
  std::vector<int> individuals;	  
  double scale = 0;
  double lambda_project = 0;
  double age_independent_mixing;
  std::vector<double> age_dependent_mixing;
};


struct workplace {
  location loc;
  double lambda_workplace = 0;
  std::vector<int> individuals; //list of indices of individuals
  std::vector<project> projects; // list of project indices in the workplace
  double Q_w = 1;
  double scale = 0;
  WorkplaceType workplace_type;
  OfficeType office_type = OfficeType::other;
  bool quarantined = false;
  double age_independent_mixing;
  std::vector<double> age_dependent_mixing;

  //age_dependent_mixing not added yet, since it is unused

  workplace(){}
  workplace(double latitude, double longitude, WorkplaceType t):
	  loc{latitude, longitude}, workplace_type(t) {}

  void set(double latitude, double longitude, WorkplaceType t){
	this->loc = {latitude, longitude};
	this->workplace_type = t;
  }

};


struct community {
  location loc;
  double lambda_community = 0;
  double lambda_community_global = 0;
  std::vector<int> individuals; //list of indices of individuals
  std::vector<int> households;  //list of households in a community
  double Q_c = 1;
  double scale = 0;
  bool quarantined = false;

  //parameter for measuring how locked down the community is
  double w_c = 1;

  int wardNo;
  community(){}
  community(double latitude, double longitude, int wardNo):
	loc{latitude, longitude}, wardNo{wardNo}{}
  void set(double latitude, double longitude, int wardNo){
	this->loc = {latitude, longitude};
	this->wardNo = wardNo;
  }
};

struct nbr_cell {
  grid_cell neighbourhood;
  std::vector<count_type> houses_list;
  bool quarantined = false;
  double lambda_nbr = 0;
  double scale = 0;

  count_type population = 0;
  count_type num_active_hospitalisations = 0;
  double access_factor = 1.0; //Corresponds to w_c in the implementation of wardwise containment
};

struct office_attendance{
  count_type number_of_entries;
  matrix<double> probabilities;
  bool attendance_new_file_type = false; //new file type gives attendance in intervals rather than in days
                                         //+ it assumes full attendance for days before intervention.
};

extern office_attendance ATTENDANCE;

// Absenteeism parameter. This may depend on the workplace type.
double psi_T(const agent& node, double cur_time);


//interpolation with a threshold
double interpolate(double start, double end, double current, double threshold);

//reset household and individual compliance flags based on compliance probability.
void set_compliance(std::vector<agent> & nodes, std::vector<house> & homes,
					double usual_compliance_probability, double hd_area_compliance_probability);

void set_nbr_cell(house &home);

//kappa_T severity calculation
double kappa_T(const agent&node, double cur_time);
#endif
