#!/bin/bash
#SBATCH --job-name=t_3840x2160
#SBATCH --partition=all
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=128
#SBATCH --output=logsTime/output_t_3840x2160.txt
#SBATCH --error=logsTime/error_t_3840x2160.txt
#SBATCH --time=10:00:00
#SBATCH --mem=64GB

# Parameters
filename="3840x2160.png"
numOfSeams=128
optimal_loadImageThreads=16
optimal_energyThreads=64
optimal_ceFirstRowCopyThreads=8
optimal_ceRowParalelizationThreads=2
optimal_arrayShiftThreads=32
optimal_writeImageThreads=64

outputFileName="times_3840x2160_threads.txt"

# Run the program 5 times
num_runs=5

# Arrays to store individual run times
declare -a run_times_real
declare -a run_times_user
declare -a run_times_system

for ((i=1; i<=$num_runs; i++)); do
    echo "Running iteration $i..."
    # Run the program and capture time
    output=$( { time (./seam_carving.out "$filename" $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads); } 2>&1 )
    
    # Extract real, user, and system times
    real_time=$(echo "$output" | awk '/real/ {print $2}')
    user_time=$(echo "$output" | awk '/user/ {print $2}')
    system_time=$(echo "$output" | awk '/sys/ {print $2}')
    
    # Convert times to seconds
    real_time_seconds=$(echo "$real_time" | awk -F 'm|s' '{print ($1 * 60) + $2}')
    user_time_seconds=$(echo "$user_time" | awk -F 'm|s' '{print ($1 * 60) + $2}')
    system_time_seconds=$(echo "$system_time" | awk -F 'm|s' '{print ($1 * 60) + $2}')
    
    # Store times in the arrays
    run_times_real[$i]=$real_time_seconds
    run_times_user[$i]=$user_time_seconds
    run_times_system[$i]=$system_time_seconds
done

# Calculate the sum of real, user, and system run times
total_real=0
total_user=0
total_system=0
for ((i=1; i<=$num_runs; i++)); do
    total_real=$(echo "$total_real + ${run_times_real[$i]}" | bc)
    total_user=$(echo "$total_user + ${run_times_user[$i]}" | bc)
    total_system=$(echo "$total_system + ${run_times_system[$i]}" | bc)
done

# Calculate the averages
average_real=$(echo "scale=2; $total_real / $num_runs" | bc)
average_user=$(echo "scale=2; $total_user / $num_runs" | bc)
average_system=$(echo "scale=2; $total_system / $num_runs" | bc)

# Save parameters to a file
echo "Parameters: $filename $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads" > resultsTime/$outputFileName

# Save average times to a file
echo -n "Average real time: $average_real s" >> resultsTime/$outputFileName
echo -n " [ " >> resultsTime/$outputFileName
for element in "${run_times_real[@]}"; do
    echo -n "$element " >> resultsTime/$outputFileName
done
echo "]" >> resultsTime/$outputFileName

echo -n "Average user time: $average_user s" >> resultsTime/$outputFileName
echo -n " [ " >> resultsTime/$outputFileName
for element in "${run_times_user[@]}"; do
    echo -n "$element " >> resultsTime/$outputFileName
done
echo "]" >> resultsTime/$outputFileName

echo -n "Average system time: $average_system s" >> resultsTime/$outputFileName
echo -n " [ " >> resultsTime/$outputFileName
for element in "${run_times_system[@]}"; do
    echo -n "$element " >> resultsTime/$outputFileName
done
echo "]" >> resultsTime/$outputFileName

