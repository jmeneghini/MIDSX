#!/bin/bash

# Check for correct number of arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <executable> <N> <output_file>"
    exit 1
fi

executable=$1
N=$2
output_file=$3

# Run the executable N times
for ((i=1; i<=N; i++)); do
    ./$executable >> $output_file
done

echo "Finished running $executable $N times. Output appended to $output_file."
