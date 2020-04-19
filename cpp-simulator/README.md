To run the simulator, you need:

1. a recent version of the `g++` compiler (version 7 or later) or `clang`
(version 11 or later should work, though we have have not tested), and
optionally `openmp` (this is to enable parallelization, and can be disabled
easily, see below in the compiling section if your installation does not provide
it),

2. The `make` program.

3. The `bash` shell.

4. The `gnuplot` program for generating some of the plots.

5. `python` pointing to a `python3` executable with `matplotlib` and `pandas`
   installed, in your execution `PATH`. (Otherwise, you will have to edit the
   `python` invocation in the shell script to point to such a `python3`
   installation.)  See the `README.md` [file](../README.md) in the root of the
   source distribution directory for one way to obtain such a `python`
   environment.

On windows, the easiest way to get these is to install `msys2` shell
from [here](https://www.msys2.org/).  However, it may be tricky to get
an MSYS installation with a `python3` capable of installing `pandas`.
You might then have to point `MSYS` to actually use your Windows
`python3` installation. Let me know if you need help.

On Mac and Linux, most of the above programs are typically already there, or
else are easy to install with a package manager.


## Compiling the code

You can now compile the code in the `cpp_simulator` directory by running:

```
make all
```

However, if you do not want to use, or do not have, the `openmp`
parallel libraries, you should instead run

```
make -f Makefile_np all
```


This will generate an executable file called `drive_simulator` in the
same directory.  This is the simulator program.


## Running the code

Please proceed with this step only after compiling the code, as described in the
previous step.

There are two ways to run the simulator.  However, before running it, you will
need to generate the input files for the simulator (corresponding to the city
you want to simulate).  Instructions for doing this are in the README.md
[file](../README.md) in the root installation directory.

### Method 1: Running `drive_simulator` directly

For this, first run the following command

```
./drive_simulator -h
```

and examine the various options available.  In particular, you need to specify
the `input_directory` as the directory were you generated the input files for
the city you want to simulate.  For more details on the other parameters
(especially the BETA parameters) see the description in the write-up.

You can now run the program after providing the parameters at the command line
in the usual way.  Options that you do not specify will be set to their default
values as described in the output of the command `./drive_simulator -h` you ran
above.  Note that the INTERVENTION parameter must be a positive integer.  The
following interventions are implemented (along with the integer value that one
needs to provide to run them).

|------+-----------------------------------------------|
| Code | Identifier for intervention                   |
|------+-----------------------------------------------|
|    0 | no_intervention,                              |
|    1 | case_isolation                                |
|    2 | home_quarantine                               |
|    3 | lockdown                                      |
|    4 | case_isolation_and_home_quarantine            |
|    5 | case_isolation_and_home_quarantine_sd_70_plus |
|    6 | lockdown_fper_ci_hq_sd_70_plus_sper_ci        |
|    7 | lockdown_fper                                 |
|    8 | ld_fper_ci_hq_sd65_sc_sper_sc_tper            |
|    9 | ld_fper_ci_hq_sd65_sc_sper                    |
|   10 | ld_fper_ci_hq_sd65_sc_oe_sper                 |
|------+-----------------------------------------------|


The output of the program will be generated in the `output_directory` that you
provide.  It consists of various CSV files (with human readable names)
containing time series data of various observables in the model.


### Method 1: Using the provided shell scripts


You can also run the code for several interventions in one go using the driver
scripts provided here.  Or example, the driver script with default parameters
for Mumbai is called `drive_simulator_mum.sh`.  To run the code in this method,
you will have to edit the driver script (say `drive_simulator_mum.sh`) where you
can specify:

1) The input parameters.

2) The directory where the output will be generated (The `output_directory_base`
	parameter in the file).  This option can also be passed to the script from
	the command line as shown below.

3) The directory in which the input files reside. (The `input_directory`
	parameter in the file.)  This option can also be passed to the script from
	the command line as shown below.

Once you have fixed the parameters, you can run the script as

```
bash drive_simulator_temp.sh [-i input_directory] [-o output_directory_base]
```

in the `cpp-simulator` directory.  If everything works, it will end
with a message indicating success after a little while (~25s per
intervention on my computer with 1,00,000 agents).

The main simulator executable is still `drive_simulator`. 

#### Viewing the output

The output can now be seen by using your favorite browser to open the
`plots.html` files in the output directories.

	- There will be one output directory per intervention (inside the directory you specified), containing the plots just for that intervention.
	- In addition, in the top level directory (the one you specified)q with plots which plot the observable variables for all the interventions on the same plot.

The per intervention directories also have the raw CSV files from
which the plots are generated.

