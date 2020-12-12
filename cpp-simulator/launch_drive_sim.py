import os
import subprocess
import time
import itertools
from multiprocessing import Pool

BASE_PATH = '/home/sharadshriram/code/iisc/covid/markov_simuls'
INPUT_PATH = F'{BASE_PATH}/staticInst/data/web_input_files/mumbai_cohorts_100K'
OUTPUT_PATH = F'{BASE_PATH}/temp_output'
SIMULATOR_PATH = './drive_simulator'

ITERATIONS_PER_CONFIG = 1
NUM_DAYS = 20

BETA_COHORT = [0.0005]  # '0.0001', '0.0005']
ISOLATION_POLICY = ['--ISOLATE_COHORTS']
CROWDING_FACTORS = [5]
COHORT_SIZES = [20]
COHORT_SEVERITY_FRACTION = [0.4]
COHORT_STRATEGY = [1]
STORE_STATE_TIME_STEP = 0
LOAD_STATE_TIME_STEP = 0
ONE_OFF_TRAVELERS_RATIO = [0.0]
# FRACTION_IN_TRAINS = [0.1, 0.5, 1.0]

MAX_CONFIGS_TO_RUN = 0
PROC_TO_RUN = 1


def make_folder_if_not_exist(path):
    '''Returns if sim should run.'''
    if not os.path.isdir(path):
        try:
            os.mkdir(path)
        except OSError:
            print("Creation of the directory %s failed" % path)
            exit(0)
        else:
            print("Successfully created output directory %s " % path)
            return True
    elif not os.path.isfile(path+'/global_params.txt'):
        return True
    else:
        return False


def launch_proc_with_config(
        jobNum, beta, cohortSize, crowdingFactor, 
        isolationPolicy, cohortSeverityFraction, cohortStrategy,
        oneOff, outputPath):
    print(F'starting process # {jobNum}')
    if not make_folder_if_not_exist(outputPath):
        print(F'Sim results exist for {outputPath}. Skipping')
        return

    sleep_duration = 23 * (jobNum % PROC_TO_RUN)
    time.sleep(sleep_duration)
    subprocess.run(args=[F'{SIMULATOR_PATH}',
                         '--SEED_FIXED_NUMBER',
                         '--NUM_DAYS', F'{NUM_DAYS}',
                         '--CITY_SW_LAT', '18.89395643371942',
                         '--CITY_NE_LAT', '19.270176667777736',
                         '--CITY_SW_LON', '72.77633295153348',
                         '--CITY_NE_LON', '72.97973149704592',
                         '--INIT_FRAC_INFECTED', '0.00001',
                         '--INIT_FIXED_NUMBER_INFECTED', '100',
                         '--MEAN_INCUBATION_PERIOD', '4.6',
                         '--MEAN_ASYMPTOMATIC_PERIOD', '0.5',
                         '--MEAN_SYMPTOMATIC_PERIOD', '5',
                         '--SYMPTOMATIC_FRACTION', '0.67',
                         '--MEAN_HOSPITAL_REGULAR_PERIOD', '8',
                         '--MEAN_HOSPITAL_CRITICAL_PERIOD', '8',
                         '--F_KERNEL_A', '2.709',
                         '--F_KERNEL_B', '1.279',
                         '--BETA_H', '0.792844',
                         '--BETA_W', '0.141709',
                         '--BETA_C', '0.0149375',
                         '--BETA_S', '0.283418',
                         '--BETA_PROJECT', '1.2753',
                         '--BETA_CLASS', '2.5507',
                         '--BETA_RANDOM_COMMUNITY', '0.1344',
                         '--BETA_NBR_CELLS', '0.1344',
                         '--BETA_TRAVEL', '0',
                         '--HD_AREA_FACTOR', '2.0',
                         '--HD_AREA_EXPONENT', '0',
                         '--INTERVENTION', '16',
                         '--output_directory', F'{outputPath}',
                         '--input_directory', F'{INPUT_PATH}',
                         '--IGNORE_ATTENDANCE_FILE',
                         '--ENABLE_NBR_CELLS',
                         '--CALIBRATION_DELAY', '1',
                         '--DAYS_BEFORE_LOCKDOWN', '2',
                         '--FIRST_PERIOD', '3',
                         '--SECOND_PERIOD', '4',
                         '--THIRD_PERIOD', '5',
                         '--OE_SECOND_PERIOD', '6',
                         '--ENABLE_TESTING',
                         '--LOCKED_COMMUNITY_LEAKAGE', '0.25',
                         '--TESTING_PROTOCOL', '2',
                         '--attendance_filename', 'mumbai_attendance.json',
                         '--testing_protocol_filename', 'testing_protocol.json',
                         '--MASK_ACTIVE',
                         '--MASK_FACTOR', '0.8',
                         '--MASK_START_DELAY', '5',
                         '--PROVIDE_INITIAL_SEED_GRAPH', '4123',
                         '--PROVIDE_INITIAL_SEED', '1723530071',
                         '--intervention_filename', '2020091_intervention_params_community_leakage_factor_1_fix_May18-31.json',
                         '--ENABLE_CONTAINMENT',
                         '--ENABLE_COHORTS',
                         '--COHORT_SIZE', F'{cohortSize}',
                         '--BETA_COHORT', F'{beta}',
                         '--CROWDING_FACTOR_COHORTS', F'{crowdingFactor}',
                         '--COHORT_SEVERITY_FRACTION', F'{cohortSeverityFraction}',
                         '--COHORT_STRATEGY', F'{cohortStrategy}',
                         '--STORE_STATE_TIME_STEP', F'{STORE_STATE_TIME_STEP}',
                         '--LOAD_STATE_TIME_STEP', F'{LOAD_STATE_TIME_STEP}',
                         '--ONE_OFF_TRAVELERS_RATIO', F'{oneOff}', 
                         F'{isolationPolicy}'],
                   stdout=open(F'{outputPath}/cout.txt', 'w'),
                   stderr=open(F'{outputPath}/cerr.txt', 'w'))


if not os.path.isdir(INPUT_PATH):
    print('INPUT DIR missing')
    exit(0)

if not os.path.isfile(SIMULATOR_PATH):
    print('SIMULATOR missing')
    exit(0)

make_folder_if_not_exist(OUTPUT_PATH)

configs = list(itertools.product(BETA_COHORT,
                                 COHORT_SIZES,
                                 CROWDING_FACTORS,
                                 ISOLATION_POLICY,
                                 COHORT_SEVERITY_FRACTION,
                                 COHORT_STRATEGY,
                                 ONE_OFF_TRAVELERS_RATIO,
                                 list(range(ITERATIONS_PER_CONFIG))))

with Pool(processes=min((os.cpu_count() - 1), PROC_TO_RUN)) as pool:
    for index, config in enumerate(configs, start=1):
        isolation_num = '0' if len(config[3]) == 0 else '1'
        config_path = F'CB_{config[0]}_CS_{config[1]}_CF_{config[2]}_ISO_{isolation_num}_CSF_{config[4]}_STRAT_{config[5]}_ONE_{config[6]}_id_{config[7]}'
        outputPath = F'{OUTPUT_PATH}/{config_path}'
        pool.apply_async(launch_proc_with_config,
                         [index, config[0], config[1], config[2], config[3], config[4], config[5], config[6], outputPath])
        if MAX_CONFIGS_TO_RUN != 0 and index >= MAX_CONFIGS_TO_RUN:
            break

    pool.close()
    pool.join()

print("\n\nALL CONFIGS COMPLETE")
