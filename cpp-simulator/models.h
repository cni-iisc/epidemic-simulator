//Copyright [2020] [Indian Institute of Science, Bangalore & Tata Institute of Fundamental Research, Mumbai]
//SPDX-License-Identifier: Apache-2.0
#ifndef MODELS_H_
#define MODELS_H_
#include <vector>
#include <random>
#include <tuple>
#include <cmath>
#include <string>

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
/*
struct intervention_params{
    count_type num_days = 0;
    double compliance = 0.9;
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
};
*/

struct svd {
  matrix<double> u, vT;
  std::vector<double> sigma;
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
  double COMPLIANCE_PROBABILITY = 1;

  count_type num_homes = 25000;
  count_type num_workplaces = 5000;
  count_type num_schools = 0;
  count_type num_communities = 198;

  count_type num_people = 100000;

  count_type NUM_DAYS = 120; //Number of days. Simulation duration
  const count_type SIM_STEPS_PER_DAY = 4; //Number of simulation steps per day.
  count_type NUM_TIMESTEPS = NUM_DAYS*SIM_STEPS_PER_DAY; //
  double INIT_FRAC_INFECTED = 0.0001; // Initial number of people infected

  double INCUBATION_PERIOD = 2.25;
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
  double INCUBATION_PERIOD_SCALE = INCUBATION_PERIOD*SIM_STEPS_PER_DAY;// 2.29 days

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
  bool IGNORE_ATTENDANCE_FILE = false;
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

  std::string intervention_filename = "intervention_params.json";
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

  void set_zero(){
	home = 0;
	work = 0;
	community = 0;
	travel = 0;
  }

  inline double sum() const {
	return home + work + community + travel;
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
	return *this;
  }

  inline lambda_incoming_data& operator*=(long double d){
	home *= d;
	work *= d;
	community *= d;
	travel *= d;
	return *this;
  }

  inline lambda_incoming_data& operator+=(const lambda_incoming_data& rhs){
	home += rhs.home;
	work += rhs.work;
	community += rhs.community;
	travel += rhs.travel;
	return *this;
  }

  inline lambda_incoming_data& operator-=(const lambda_incoming_data& rhs){
	home -= rhs.home;
	work -= rhs.work;
	community -= rhs.community;
	travel -= rhs.travel;
	return *this;
  }

  inline void mean_update(const lambda_incoming_data& update, count_type num){
	home += (update.home - home)/num;
	work += (update.work - work)/num;
	community += (update.community - community)/num;
	travel += (update.travel - travel)/num;
  }
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
  double lambda = 0;

  double kappa_T = 1;
  double psi_T = 0;
  double funct_d_ck;

  WorkplaceType workplace_type;
  //one of school, office, or home
  OfficeType office_type = OfficeType::other;
  
  lambda_incoming_data lambda_incoming;
  //infectiousness from home, workplace, community, travel as seen by
  //individual


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
};


struct house{
  location loc;
  grid_cell neighbourhood;
  double lambda_home = 0;
  std::vector<int> individuals; //list of indices of individuals
  double Q_h = 1;

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

  //age_dependent_mixing not added yet, since it is unused
  house(){}
  house(double latitude, double longitude, bool compliance):
	loc{latitude, longitude}, compliant(compliance) {}

  void set(double latitude, double longitude, bool compliance, double non_compl_metric){
    this->loc = {latitude, longitude};
    this->compliant = compliance;
    this->non_compliance_metric = non_compl_metric;
  }
};


struct workplace {
  location loc;
  double lambda_workplace = 0;
  std::vector<int> individuals; //list of indices of individuals
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
};

struct office_attendance{
  count_type number_of_entries;
  matrix<double> probabilities;
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
