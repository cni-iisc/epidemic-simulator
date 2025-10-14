import os
import filecmp

os.chdir(os.path.dirname(__file__))


def copy_reference_files(regressions_to_update):
    for reference_directory in regressions_to_update:
        for reference_file in os.listdir(os.path.join('reference_files',reference_directory)):
            ref_file=os.path.join('reference_files',reference_directory,reference_file)
            test_file =os.path.join('output_files',reference_directory,reference_file)
            if(os.path.exists(test_file)):
                if(not filecmp.cmp(ref_file,test_file)):
                    temp= ref_file +" "+ test_file + " differ."
                    print(temp)
                    print("Copying ", test_file, "to", ref_file)
                    os.system('cp ' + test_file + ' ' + ref_file)
            else:
                print(test_file, " does not exist.")

'''
regressions_to_update = ['intervention_00',
'intervention_00_file_read',
'intervention_01',
'intervention_01_file_read',
'intervention_02',
'intervention_02_file_read',
'intervention_03',
'intervention_03_file_read',
'intervention_04',
'intervention_04_file_read',
'intervention_05',
'intervention_05_file_read',
'intervention_06',
'intervention_06_file_read',
'intervention_07',
'intervention_07_file_read',
'intervention_08',
'intervention_08_file_read',
'intervention_09',
'intervention_09_file_read',
'intervention_10',
'intervention_10_file_read',
'intervention_11',
'intervention_11_file_read',
'intervention_12',
'intervention_12_file_read',
'intervention_13',
'intervention_13_file_read',
'intervention_14',
'intervention_14_enabled',
'intervention_14_file_read',
'intervention_15',
'intervention_15_enabled',
'intervention_15_file_read',
'test_001',
]
'''
copy_reference_files(regressions_to_update)
