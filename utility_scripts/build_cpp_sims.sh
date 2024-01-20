#!/bin/bash

# This script builds the C++ simulation executables for the project.

# Set the path to the directory containing the source files.
CPP_SIMS_DIR=./cpp_simulations

# Get dir of this script.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd "${SCRIPT_DIR}"
cd .. # Go up to the project root directory.
pwd || echo

for sim_dir in "${CPP_SIMS_DIR}/"*; do
    # Get the name of the simulation.
    sim_name=$(basename "$sim_dir")

    # Build the simulation.
    echo "Building $sim_name..."
    mkdir "${sim_dir}/build"
    cd "${sim_dir}/build"
    cmake ..
    make
    cd -
done
