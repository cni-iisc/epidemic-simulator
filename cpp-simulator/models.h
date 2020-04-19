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
   ld_fper_ci_hq_sd65_sc_oe_sper = 10
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
	
  
  const double INCUBATION_PERIOD_SHAPE = 2.3;
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
  double P_TRAIN = 0.9;
  //Probability with which an agent takes a train

  //Multiplicative fatcor for infection rates in high density areas
  double HD_AREA_FACTOR = 2.0;

  double HD_AREA_EXPONENT = 0.3;

  //Lockdown periods
  double FIRST_PERIOD = 21;
  double SECOND_PERIOD = 21;
  double THIRD_PERIOD = 42;
  double OE_SECOND_PERIOD = 30;
  
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
  

  //Input and output
  std::string input_base;

  //Status
  count_type INIT_ACTUALLY_INFECTED = 0;

  //Calibration
  double CALIBRATION_DELAY = 22; //Assuming Simulator starts on March 1
  double DAYS_BEFORE_LOCKDOWN = 24; //March 1 - March 24
  double NUM_DAYS_BEFORE_INTERVENTIONS = CALIBRATION_DELAY + DAYS_BEFORE_LOCKDOWN;
};
extern global_params GLOBAL;

// return a random compliance based on GLOBAL.compliance_probability
inline bool compliance(){
  return bernoulli(GLOBAL.COMPLIANCE_PROBABILITY);
}

//Age groups (5-years)

const int NUM_AGE_GROUPS = 16;
inline int get_age_group(int age){
  int age_group = age/5;
  return std::min(age_group, NUM_AGE_GROUPS - 1);
}

// Age index for STATE_TRAN matrix
int get_age_index(int age);
double zeta(int age);
double f_kernel(double dist);


// End of global parameters

struct location{
  double lat, lon; //latitude and longitude, in degrees
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

//Default workplace value for homebound individuals.
const int WORKPLACE_HOME = -1;

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

  bool infective = false;

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
  std::vector<double> lambda_incoming;
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


  //Transporation
  bool has_to_travel = false; //does the agent take a train to go to
							  //work?
  double commute_distance = 0; //in km

  bool hd_area_resident = false;
  //Multiplication factor for high population density areas, such as slums
  double hd_area_factor = 1.0;
  double hd_area_exponent = 0;
  //only used if in the input file, some individuals are assigned to
  //slums or other high population density areas
  
  agent(){}
  // Is the agent curently traveling?
  inline bool travels() const {
  return has_to_travel
	&& !((quarantined && compliant)
		 || infection_status == Progression::hospitalised
		 || infection_status == Progression::critical
		 || infection_status == Progression::dead);
  }
};


struct house{
  location loc;
  double lambda_home = 0;
  std::vector<int> individuals; //list of indices of individuals
  double Q_h = 1;
  double scale = 0;
  bool compliant;
  bool quarantined = false;
  double age_independent_mixing = 0;
  //age_dependent_mixing not added yet, since it is unused
  house(){}
  house(double latitude, double longitude, bool compliance):
	loc{latitude, longitude}, compliant(compliance) {}

  void set(double latitude, double longitude, bool compliance){
	this->loc = {latitude, longitude};
	this->compliant = compliance;
  }
};


struct workplace {
  location loc;
  double lambda_workplace = 0;
  std::vector<int> individuals; //list of indices of individuals
  double Q_w = 1;
  double scale = 0;
  WorkplaceType workplace_type;
  bool quarantined = false;
  double age_independent_mixing = 0;
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

  int wardNo;
  community(){}
  community(double latitude, double longitude, int wardNo):
	loc{latitude, longitude}, wardNo{wardNo}{}
  void set(double latitude, double longitude, int wardNo){
	this->loc = {latitude, longitude};
	this->wardNo = wardNo;
  }
};



// Absenteeism parameter. This may depend on the workplace type.
double psi_T(const agent& node, double cur_time);



#endif
