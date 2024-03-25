#!/bin/bash
#SBATCH --job-name=sc_720x480
#SBATCH --partition=all
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=128
#SBATCH --output=logs/output_sc_720x480.txt
#SBATCH --error=logs/error_sc_720x480.txt
#SBATCH --time=01:00:00
#SBATCH --mem=64GB


# Define parameters
filename="720x480.png"
numOfSeams=128

output_file="results/${filename%.png}_${numOfSeams}_runs.txt"
optimal_file="results/${filename%.png}_${numOfSeams}_optimal.txt"

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

# Find optimal value for loadImageThreads (tries other values, if they are faster, optimal value is updated)
min_loadImageTime=$(run_program_loadImage $optimal_loadImageThreads)

for loadImageThreads in "${other_loadImageThreads[@]}"; do
    time=$(run_program_loadImage $loadImageThreads)
    if (($time < $min_loadImageTime)); then
        min_loadImageTime=$time
        optimal_loadImageThreads=$loadImageThreads
    fi
done

echo "Optimal loadImageThreads: $optimal_loadImageThreads"

# Find optimal value for energyThreads (tries other values, if they are faster, optimal value is updated)
min_energyTime=$(run_program_energy $optimal_energyThreads)

for energyThreads in "${other_energyThreads[@]}"; do
    time=$(run_program_energy $energyThreads)
    if (($time < $min_energyTime)); then
        min_energyTime=$time
        optimal_energyThreads=$energyThreads
    fi
done

echo "Optimal energyThreads: $optimal_energyThreads"

# Find optimal value for ceFirstRowCopyThreads (tries other values, if they are faster, optimal value is updated)
min_ceRowCopyTime=$(run_program_ceRowCopy $optimal_ceFirstRowCopyThreads)

for ceFirstRowCopyThreads in "${other_ceFirstRowCopyThreads[@]}"; do
    time=$(run_program_ceRowCopy $ceFirstRowCopyThreads)
    if (($time < $min_ceRowCopyTime)); then
        min_ceRowCopyTime=$time
        optimal_ceFirstRowCopyThreads=$ceFirstRowCopyThreads
    fi
done

echo "Optimal ceFirstRowCopyThreads: $optimal_ceFirstRowCopyThreads"

# Find optimal value for ceRowParalelizationThreads (tries other values, if they are faster, optimal value is updated)
min_ceRowParalelizationTime=$(run_program_ceRowParalelization $optimal_ceRowParalelizationThreads)

for ceRowParalelizationThreads in "${other_ceRowParalelizationThreads[@]}"; do
    time=$(run_program_ceRowParalelization $ceRowParalelizationThreads)
    if (($time < $min_ceRowParalelizationTime)); then
        min_ceRowParalelizationTime=$time
        optimal_ceRowParalelizationThreads=$ceRowParalelizationThreads
    fi
done

echo "Optimal ceRowParalelizationThreads: $optimal_ceRowParalelizationThreads"

# Find optimal value for arrayShiftThreads (tries other values, if they are faster, optimal value is updated)
min_removeSeamTime=$(run_program_arrayShift $optimal_arrayShiftThreads)

for arrayShiftThreads in "${other_arrayShiftThreads[@]}"; do
    time=$(run_program_arrayShift $arrayShiftThreads)
    if (($time < $min_removeSeamTime)); then
        min_removeSeamTime=$time
        optimal_arrayShiftThreads=$arrayShiftThreads
    fi
done

echo "Optimal arrayShiftThreads: $optimal_arrayShiftThreads"

# Find optimal value for writeImageThreads (tries other values, if they are faster, optimal value is updated)
min_writeImageTime=$(run_program_writeImage $optimal_writeImageThreads)

for writeImageThreads in "${other_writeImageThreads[@]}"; do
    time=$(run_program_writeImage $writeImageThreads)
    if (($time < $min_writeImageTime)); then
        min_writeImageTime=$time
        optimal_writeImageThreads=$writeImageThreads
    fi
done

echo "Optimal writeImageThreads: $optimal_writeImageThreads"

# Save optimal values to a file
echo "Optimal loadImageThreads: $optimal_loadImageThreads" >> $optimal_file
echo "Optimal energyThreads: $optimal_energyThreads" >> $optimal_file
echo "Optimal ceFirstRowCopyThreads: $optimal_ceFirstRowCopyThreads" >> $optimal_file
echo "Optimal ceRowParalelizationThreads: $optimal_ceRowParalelizationThreads" >> $optimal_file
echo "Optimal arrayShiftThreads: $optimal_arrayShiftThreads" >> $optimal_file
echo "Optimal writeImageThreads: $optimal_writeImageThreads" >> $optimal_file

# save optimal values to a file in one line
echo "$filename $numOfSeams $optimal_loadImageThreads $optimal_energyThreads $optimal_ceFirstRowCopyThreads $optimal_ceRowParalelizationThreads $optimal_arrayShiftThreads $optimal_writeImageThreads" >> $optimal_file
```


