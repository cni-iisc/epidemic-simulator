cmake_minimum_required(VERSION 3.2)

project(EpidemicSimulator VERSION 1.0)

execute_process(COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND 
    (git status --untracked-files=no --porcelain | grep -q .) && echo dirty || echo clean
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TREE_STATE
    OUTPUT_STRIP_TRAILING_WHITESPACE)


add_definitions(-DGIT_HASH="${GIT_HASH}")
add_definitions(-DGIT_TREE_STATE="${GIT_TREE_STATE}")

# Global options
option(WITH_OPENMP "Enable parallelization using openmp" OFF)

# Configuration

set(CMAKE_CXX_STANDARD 14)
if(WITH_OPENMP)
    find_package(Threads REQUIRED)
    find_package(OpenMP REQUIRED)
endif()

# Timings
add_definitions(-DTIMING)

# Set the random number generator.
add_definitions(-DMERSENNE_TWISTER)

# global include paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/cxxopts-2.2.0/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/
    )

add_executable(drive_simulator 
    drive_simulator.cc
    initializers.cc
    intervention_primitives.cc
    interventions.cc
    models.cc
    outputs.cc
    simulator.cc
    updates.cc)

if(WITH_OPENMP)
    target_compile_options(drive_simulator PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(drive_simulator 
        OpenMP::OpenMP_CXX
        Threads::Threads)
endif()


