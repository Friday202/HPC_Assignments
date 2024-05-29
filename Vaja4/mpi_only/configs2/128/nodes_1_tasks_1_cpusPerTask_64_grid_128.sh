#!/bin/sh
#SBATCH --reservation=fri
#SBATCH --job-name=lenia_MPI
#SBATCH --time=10:00:00 
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=1
#SBATCH --threads-per-core=1
#SBATCH --cpus-per-task=1
#SBATCH --output=logs2/128/nodes_1_tasks_1_cpusPerTask_64_grid_128.log

module load OpenMPI
mpicc main.c lenia.c gifenc.c orbium.c -O2 -o lenia_MPI -fopenmp -lm -lmpi

# OpenMP environment variables

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

mpirun -np $SLURM_NTASKS ./lenia_MPI 128
