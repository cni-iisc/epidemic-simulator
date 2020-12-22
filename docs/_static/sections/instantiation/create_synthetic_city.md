# Generating synthetic city

With these packages installed, we are now ready to actually build the city.

The demographics, employment etc. data is present in the folder ``staticInst/data/base/[city name]``. We will work with Mumbai and generate the city.

```bash
(edaDev) $ ~/tutorial/markov_simuls > cd staticInst
(edaDev) $ ~/tutorial/markov_simuls/staticInst > python CityGen.py -h
usage: CityGen.py [-h] [-n N] [-i I] [-o O] [--validate] [-s S]

Create mini-city for COVID-19 simulation

optional arguments:
  -h, --help  show this help message and exit
  -n N        target population
  -i I        input folder
  -o O        output folder
  --validate  script for validation plots on
  -s S        [for debug] restore random seed from folder
```

With the syntax in mind, let us go ahead and build the city (of 150k size).

```bash
(edaDev) $ ~/tutorial/markov_simuls/staticInst > python CityGen.py -n 150000 -i data/base/mumbai -o data/mumbai_150k --validate
input_folder: data/base/mumbai
output_folder: data/mumbai_150k

(Distance kernel parameters provided.)
createHouses                  ...	done. (6242 ms)
populateHouses                ...	done. (22332 ms)
assignSchools                 ...	done. (287 ms)
assignWorkplaces              ...	done. (6630 ms)

City: mumbai
Population: 150071
Number of wards: 48
Has slums: True

Number of houses: 33259
Number of schools: 107
Number of workplaces: 2899
Number of workers: 60606

dump_files                    ...	done. (1239 ms)
validate_ages                 ...	done. (452 ms)
validate_householdsizes       ...	done. (158 ms)
validate_schoolsizes          ...	done. (162 ms)
validate_workplacesizes       ...	done. (801 ms)
validate_commutedistances     ...	done. (1819 ms)
```


We now have a 150k version of Mumbai instantiated in ``staticInst/data/mumbai_150k``.
