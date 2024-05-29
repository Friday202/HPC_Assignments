#!/bin/sh
#SBATCH --reservation=fri
#SBATCH --job-name=lenia_MPI
#SBATCH --time=10:00:00 
#SBATCH --ntasks=8
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8
#SBATCH --cpus-per-task=16
#SBATCH --output=logs/1024/nodes_1_tasks_8_cpusPerTask_8_grid_1024.log

module load OpenMPI
mpicc main.c lenia.c gifenc.c orbium.c -O2 -o lenia_MPI -fopenmp -lm -lmpi

# OpenMP environment variables

export OMP_NUM_THREADS=8

mpirun -np $SLURM_NTASKS ./lenia_MPI 1024
