# The simulator

You can now compile the code in the `cpp_simulator` directory by running:

```bash
make all
```

However, if you do not want to use, or do not have, the `openmp`
parallel libraries, you should instead run

```bash
make -f Makefile_np all
```
By default, the load--store feature is disabled in the simulator, for backward compatibility. To use this feature, use the following command in `make`.

With `openmp`:

```bash
make enable_proto=yes all
```

Without `openmp`:

```bash
make enable_proto=yes -f Makefile_np all
```



This will generate an executable file called `drive_simulator` in the
same directory.  This is the simulator program.


## Running the code

Please proceed with this step only after compiling the code, as described in the
previous step.

There are two ways to run the simulator.  However, before running it, you will
need to generate the input files for the simulator (corresponding to the city
you want to simulate).  Instructions for doing this are in the [Generating synthetic city](../instantiation/create_synthetic_city.md) page.

### Method 1: Running `drive_simulator` directly

For this, first run the following command

```bash
./drive_simulator -h
```

and examine the various options available.  In particular, you need to specify
the `input_directory` as the directory were you generated the input files for
the city you want to simulate.  For more details on the other parameters
(especially the BETA parameters) see the [parameter description](parameter.md).

You can now run the program after providing the parameters at the command line
in the usual way.  Options that you do not specify will be set to their default
values as described in the output of the command `./drive_simulator -h` you ran
above.  Note that the INTERVENTION parameter must be a positive integer.  

The output of the program will be generated in the `output_directory` that you
provide.  It consists of various CSV files (with human readable names)
containing time series data of various observables in the model.


### Method 1: Using the provided shell scripts


You can also run the code for several interventions in one go using the driver
scripts provided here.  Or example, the driver script with default parameters
for Mumbai is called `drive_simulator_mum.sh`.  To run the code in this method,
you will have to edit the driver script (say `drive_simulator_mum.sh`) where you
can specify:

1. The input parameters.

2. The directory where the output will be generated (The `output_directory_base`
	parameter in the file).  This option can also be passed to the script from
	the command line as shown below.

3. The directory in which the input files reside. (The `input_directory`
	parameter in the file.)  This option can also be passed to the script from
	the command line as shown below.

Once you have fixed the parameters, you can run the script as

```bash
bash drive_simulator_temp.sh [-i input_directory] [-o output_directory_base]
```

in the `cpp-simulator` directory.  If everything works, it will end
with a message indicating success after a little while (~25s per
intervention on my computer with 1,00,000 agents).

The main simulator executable is still `drive_simulator`. 

#### Viewing the output

The binary `drive_simulator` generates output as a set of csv files with information during each timestep. It also generates a `gnuplot` script file called `gnuplot_script.gnuplot`. If you have `gnuplot` installed, you may go to the folder and run `gnuplot gnuplot_script.gnuplot` and it will generate some prepecified plots that you can also view on using your favorite browser to open the `plots.html` files in the output directories.

- There will be one output directory per intervention (inside the directory you specified), containing the plots just for that intervention.
- In addition, in the top level directory (the one you specified) with plots which plot the observable variables for all the interventions on the same plot.

The per intervention directories also have the raw CSV files from
which the plots are generated.