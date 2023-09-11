#!/bin/bash

# Check for correct number of arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <N> <output_folder>"
    exit 1
fi

energies=("30keV" "30kVp" "100keV" "100kVp")
air_kermas=("hvl" "qvl")


N=$1
output_folder=$2

json_file="hvl.json"
executable_folder="cmake-build-release"
file_dir="cpp_simulations/half_value_layer/"

# set cwd to root
cd /home/john/Documents/MCXrayTransport

for energy in "${energies[@]}"; do
    executable="$file_dir$executable_folder/hvl_${energy}"
    for air_kerma in "${air_kermas[@]}"; do
        # modify json file
        jq '.voxel_grids[0].file_path = "../../data/voxels/al_'${air_kerma}'_'${energy}'.nii.gz"' "$file_dir$json_file" > "${file_dir}temp_$json_file"
        mv "${file_dir}temp_$json_file" "$file_dir$json_file"
        # with json file modified, run executable N times
        for ((i=1; i<=N; i++)); do
            echo "Running $executable $i/$N"
            ./$executable >> "$output_folder/Al_${energy}_AK_${air_kerma}.txt"
        done
    done
done

# echo finished
echo "Finished running $N simulations for each energy and air kerma level"
