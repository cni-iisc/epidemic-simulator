# Installation

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
   installation.)  

On windows, the easiest way to get these is to install `msys2` shell
from [here](https://www.msys2.org/).  However, it may be tricky to get
an MSYS installation with a `python3` capable of installing `pandas`.
You might then have to point `MSYS` to actually use your Windows
`python3` installation. Let me know if you need help.

On Mac and Linux, most of the above programs are typically already there, or
else are easy to install with a package manager.

You can then follow the set-up instructions to install the other required python packages in the [setting up the python environment](/sections/instantiation/setting_up_py_env.md) page. 

## Enabling feature to load--store simulator state
The simulator has a feature where the state of the infection spread among the agents in the syntheic city can be saved and used at a later simulation run.
The store and load feature is implemented by storing the agents' state into a [protobuf](https://developers.google.com/protocol-buffers/) and you will need to install protobuf for cpp in your development or simulation environment if you plan to use the load--store feature.

You can setup protobuf for cpp on a Debian/Ubuntu based Linux distribution with the following commands. Please note that, the simulator uses `protobuf v.3.13.0`.
- Install prerequisite software: `sudo apt install automake cmake autoconf libtool make g++`
- Download Protobuf v.3.13.0 as a compressed tarball: `wget https://github.com/protocolbuffers/protobuf/releases/download/v3.13.0/protobuf-cpp-3.13.0.tar.gz`
- Extract the tarball: `tar -xvf protobuf-cpp-3.13.0.tar.gz`
- Go to the extracted directory: `cd protobuf-3.13.0`
- Run the command: `./configure`
- Run the command: `make`
- Run the command: `make check`
- Run the command: `sudo make install`
- Run the command: `sudo ldconfig`

> **Note**: You will need to setup the same version of protobuf while running simulations also.