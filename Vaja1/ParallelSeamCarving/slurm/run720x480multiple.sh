#!/bin/bash
#SBATCH --job-name=scm_720x480
#SBATCH --partition=all
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=128
#SBATCH --output=logsMult/output_sc_720x480.txt
#SBATCH --error=logsMult/error_sc_720x480.txt
#SBATCH --time=01:00:00
#SBATCH --mem=64GB


# Define parameters
filename="720x480.png"
numOfSeams=128

output_file="resultsMult/${filename%.png}_${numOfSeams}_runs.txt"
optimal_file="resultsMult/${filename%.png}_${numOfSeams}_optimal.txt"

# Function to run the program with specific parameters and return loadImageTime
run_program_loadImage() {
    loadImageThreads=$1

    # Run the program with the given parameters
    output=$( { time -p ./seam_carving.out "$filename" $numOfSeams $loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads; } 2>&1 )

    #extract all time measurements and save them to one line separated by a space 
    time_real=$(echo "$output" | grep real | awk '{print $2}')
    time_user=$(echo "$output" | grep user | awk '{print $2}')
    time_sys=$(echo "$output" | grep sys | awk '{print $2}')

    #remove all time measurements from the output
    output=$(echo "$output" | grep -v real | grep -v user | grep -v sys)

    # save all input and output values to a file output_file
    echo "$filename $numOfSeams $loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads | $output | r:$time_real u:$time_user s:$time_sys" >> $output_file

    # Extract loadImageTime and return it
    loadImageTime=$(echo "$output" | awk '{print $1}')
    echo $loadImageTime
}

# Function to run the program with specific parameters and return avgEnergyTime
run_program_energy() {
    energyThreads=$1

    # Run the program with the given parameters
    output=$( { time -p ./seam_carving.out "$filename" $numOfSeams $optimal_loadImageThreads $energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads; } 2>&1 )

    #extract all time measurements and save them to one line separated by a space
    time_real=$(echo "$output" | grep real | awk '{print $2}')
    time_user=$(echo "$output" | grep user | awk '{print $2}')
    time_sys=$(echo "$output" | grep sys | awk '{print $2}')

    #remove all time measurements from the output
    output=$(echo "$output" | grep -v real | grep -v user | grep -v sys)

    # save all input and output values to a file output_file
    echo "$filename $numOfSeams $optimal_loadImageThreads $energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads | $output | r:$time_real u:$time_user s:$time_sys" >> $output_file

    # Extract avgEnergyTime and return it
    avgEnergyTime=$(echo "$output" | awk '{print $2}')
    echo $avgEnergyTime
}

# Function to run the program with specific parameters and return avgCeTime
run_program_ceRowCopy() {
    ceFirstRowCopyThreads=$1

    # Run the program with the given parameters
    output=$( { time -p ./seam_carving.out "$filename" $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads; } 2>&1 )

    #extract all time measurements and save them to one line separated by a space
    time_real=$(echo "$output" | grep real | awk '{print $2}')
    time_user=$(echo "$output" | grep user | awk '{print $2}')
    time_sys=$(echo "$output" | grep sys | awk '{print $2}')

    #remove all time measurements from the output
    output=$(echo "$output" | grep -v real | grep -v user | grep -v sys)

    # save all input and output values to a file output_file
    echo "$filename $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads | $output | r:$time_real u:$time_user s:$time_sys" >> $output_file

    # Extract avgCeTime and return it
    avgCeTime=$(echo "$output" | awk '{print $3}')
    echo $avgCeTime
}

# Function to run the program with specific parameters and return avgCeRowParalelizationTime
run_program_ceRowParalelization() {
    ceRowParalelizationThreads=$1

    # Run the program with the given parameters
    output=$( { time -p ./seam_carving.out "$filename" $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads; } 2>&1 )

    #extract all time measurements and save them to one line separated by a space
    time_real=$(echo "$output" | grep real | awk '{print $2}')
    time_user=$(echo "$output" | grep user | awk '{print $2}')
    time_sys=$(echo "$output" | grep sys | awk '{print $2}')

    #remove all time measurements from the output
    output=$(echo "$output" | grep -v real | grep -v user | grep -v sys)

    # save all input and output values to a file output_file
    echo "$filename $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads | $output | r:$time_real u:$time_user s:$time_sys" >> $output_file

    # Extract avgCeRowParalelizationTime and return it
    avgCeRowParalelizationTime=$(echo "$output" | awk '{print $4}')
    echo $avgCeRowParalelizationTime
}

# Function to run the program with specific parameters and return avgCeRowParalelizationTime
run_program_arrayShift() {
    arrayShiftThreads=$1

    # Run the program with the given parameters
    output=$( { time -p ./seam_carving.out "$filename" $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $arrayShiftThreads $optimal_writeImageThreads; } 2>&1 )

    #extract all time measurements and save them to one line separated by a space
    time_real=$(echo "$output" | grep real | awk '{print $2}')
    time_user=$(echo "$output" | grep user | awk '{print $2}')
    time_sys=$(echo "$output" | grep sys | awk '{print $2}')

    #remove all time measurements from the output
    output=$(echo "$output" | grep -v real | grep -v user | grep -v sys)

    # save all input and output values to a file output_file
    echo "$filename $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $arrayShiftThreads $optimal_writeImageThreads | $output | r:$time_real u:$time_user s:$time_sys" >> $output_file

    # Extract avgRemoveSeamTime and return it
    avgRemoveSeamTime=$(echo "$output" | awk '{print $5}')
    echo $avgRemoveSeamTime
}


# Function to run the program with specific parameters and return writeImageTime
run_program_writeImage() {
    writeImageThreads=$1

    # Run the program with the given parameters
    output=$( { time -p ./seam_carving.out "$filename" $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $writeImageThreads; } 2>&1 )

    #extract all time measurements and save them to one line separated by a space
    time_real=$(echo "$output" | grep real | awk '{print $2}')
    time_user=$(echo "$output" | grep user | awk '{print $2}')
    time_sys=$(echo "$output" | grep sys | awk '{print $2}')

    #remove all time measurements from the output
    output=$(echo "$output" | grep -v real | grep -v user | grep -v sys)

    # save all input and output values to a file output_file
    echo "$filename $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $writeImageThreads | $output | r:$time_real u:$time_user s:$time_sys" >> $output_file

    # Extract writeImageTime and return it
    writeImageTime=$(echo "$output" | awk '{print $6}')
    echo $writeImageTime
}




# Create or truncate output files
> $output_file
> $optimal_file

# define optimal values (values that are not currently optimized will be set to this values)
optimal_loadImageThreads=6
other_loadImageThreads=(2 4 8 12 16 32)

optimal_energyThreads=32
other_energyThreads=(16 48 64 96 128)

optimal_ceFirstRowCopyThreads=4
other_ceFirstRowCopyThreads=(1 2 6 8 12)

optimal_ceRowParalelizationThreads=2
other_ceRowParalelizationThreads=(1 4 6 8 16)

optimal_arrayShiftThreads=8
other_arrayShiftThreads=(1 4 6 12 16 32)

optimal_writeImageThreads=8
other_writeImageThreads=(4 6 8 12 16 32)

runs=5

# Find optimal value for loadImageThreads (tries other values, if they are faster, optimal value is updated)
min_loadImageTime=0
for i in $(seq 1 $runs); do
    loadImageTime=$(run_program_loadImage $optimal_loadImageThreads)
    min_loadImageTime=$((min_loadImageTime + loadImageTime))
done
min_loadImageTime=$((min_loadImageTime / runs))


for loadImageThreads in "${other_loadImageThreads[@]}"; do
    time=0
    for i in $(seq 1 $runs); do
        loadImageTime=$(run_program_loadImage $loadImageThreads)
        time=$((time + loadImageTime))
    done
    time=$((time / runs))
    if (($time < $min_loadImageTime)); then
        min_loadImageTime=$time
        optimal_loadImageThreads=$loadImageThreads
    fi
done

echo "Optimal loadImageThreads: $optimal_loadImageThreads | avgLoadImageTime_runs=$runs: $min_loadImageTime"

# Find optimal value for energyThreads (tries other values, if they are faster, optimal value is updated)
min_energyTime=0
for i in $(seq 1 $runs); do
    energyTime=$(run_program_energy $optimal_energyThreads)
    min_energyTime=$((min_energyTime + energyTime))
done
min_energyTime=$((min_energyTime / runs))

for energyThreads in "${other_energyThreads[@]}"; do
    time=0
    for i in $(seq 1 $runs); do
        energyTime=$(run_program_energy $energyThreads)
        time=$((time + energyTime))
    done
    time=$((time / runs))
    if (($time < $min_energyTime)); then
        min_energyTime=$time
        optimal_energyThreads=$energyThreads
    fi
done

echo "Optimal energyThreads: $optimal_energyThreads | avgEnergyTime_runs=$runs: $min_energyTime"

# Find optimal value for ceFirstRowCopyThreads (tries other values, if they are faster, optimal value is updated)
min_ceFirstRowCopyTime=0
for i in $(seq 1 $runs); do
    ceFirstRowCopyTime=$(run_program_ceRowCopy $optimal_ceFirstRowCopyThreads)
    min_ceFirstRowCopyTime=$((min_ceFirstRowCopyTime + ceFirstRowCopyTime))
done
min_ceFirstRowCopyTime=$((min_ceFirstRowCopyTime / runs))

for ceFirstRowCopyThreads in "${other_ceFirstRowCopyThreads[@]}"; do
    time=0
    for i in $(seq 1 $runs); do
        ceFirstRowCopyTime=$(run_program_ceRowCopy $ceFirstRowCopyThreads)
        time=$((time + ceFirstRowCopyTime))
    done
    time=$((time / runs))
    if (($time < $min_ceFirstRowCopyTime)); then
        min_ceFirstRowCopyTime=$time
        optimal_ceFirstRowCopyThreads=$ceFirstRowCopyThreads
    fi
done

echo "Optimal ceFirstRowCopyThreads: $optimal_ceFirstRowCopyThreads | avgCeFirstRowCopyTime_runs=$runs: $min_ceFirstRowCopyTime"

# Find optimal value for ceRowParalelizationThreads (tries other values, if they are faster, optimal value is updated)
min_ceRowParalelizationTime=0
for i in $(seq 1 $runs); do
    ceRowParalelizationTime=$(run_program_ceRowParalelization $optimal_ceRowParalelizationThreads)
    min_ceRowParalelizationTime=$((min_ceRowParalelizationTime + ceRowParalelizationTime))
done
min_ceRowParalelizationTime=$((min_ceRowParalelizationTime / runs))

for ceRowParalelizationThreads in "${other_ceRowParalelizationThreads[@]}"; do
    time=0
    for i in $(seq 1 $runs); do
        ceRowParalelizationTime=$(run_program_ceRowParalelization $ceRowParalelizationThreads)
        time=$((time + ceRowParalelizationTime))
    done
    time=$((time / runs))
    if (($time < $min_ceRowParalelizationTime)); then
        min_ceRowParalelizationTime=$time
        optimal_ceRowParalelizationThreads=$ceRowParalelizationThreads
    fi
done

echo "Optimal ceRowParalelizationThreads: $optimal_ceRowParalelizationThreads | avgCeRowParalelizationTime_runs=$runs: $min_ceRowParalelizationTime"

# Find optimal value for arrayShiftThreads (tries other values, if they are faster, optimal value is updated)
min_arrayShiftTime=0
for i in $(seq 1 $runs); do
    arrayShiftTime=$(run_program_arrayShift $optimal_arrayShiftThreads)
    min_arrayShiftTime=$((min_arrayShiftTime + arrayShiftTime))
done
min_arrayShiftTime=$((min_arrayShiftTime / runs))

for arrayShiftThreads in "${other_arrayShiftThreads[@]}"; do
    time=0
    for i in $(seq 1 $runs); do
        arrayShiftTime=$(run_program_arrayShift $arrayShiftThreads)
        time=$((time + arrayShiftTime))
    done
    time=$((time / runs))
    if (($time < $min_arrayShiftTime)); then
        min_arrayShiftTime=$time
        optimal_arrayShiftThreads=$arrayShiftThreads
    fi
done

echo "Optimal arrayShiftThreads: $optimal_arrayShiftThreads | avgArrayShiftTime_runs=$runs: $min_arrayShiftTime"

# Find optimal value for writeImageThreads (tries other values, if they are faster, optimal value is updated)
min_writeImageTime=0
for i in $(seq 1 $runs); do
    writeImageTime=$(run_program_writeImage $optimal_writeImageThreads)
    min_writeImageTime=$((min_writeImageTime + writeImageTime))
done
min_writeImageTime=$((min_writeImageTime / runs))

for writeImageThreads in "${other_writeImageThreads[@]}"; do
    time=0
    for i in $(seq 1 $runs); do
        writeImageTime=$(run_program_writeImage $writeImageThreads)
        time=$((time + writeImageTime))
    done
    time=$((time / runs))
    if (($time < $min_writeImageTime)); then
        min_writeImageTime=$time
        optimal_writeImageThreads=$writeImageThreads
    fi
done

echo "Optimal writeImageThreads: $optimal_writeImageThreads | avgWriteImageTime_runs=$runs: $min_writeImageTime"

# Save optimal values to a file
echo "Optimal loadImageThreads: $optimal_loadImageThreads" >> $optimal_file
echo "Optimal energyThreads: $optimal_energyThreads" >> $optimal_file
echo "Optimal ceFirstRowCopyThreads: $optimal_ceFirstRowCopyThreads" >> $optimal_file
echo "Optimal ceRowParalelizationThreads: $optimal_ceRowParalelizationThreads" >> $optimal_file
echo "Optimal arrayShiftThreads: $optimal_arrayShiftThreads" >> $optimal_file
echo "Optimal writeImageThreads: $optimal_writeImageThreads" >> $optimal_file

# save optimal values to a file in one line
echo "$filename $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads" >> $optimal_file


