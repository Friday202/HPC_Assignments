#!/bin/sh
#SBATCH --reservation=fri
#SBATCH --job-name=lenia_MPI
#SBATCH --time=10:00:00 
#SBATCH --ntasks=16
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --cpus-per-task=8
#SBATCH --output=logs/1024/nodes_1_tasks_16_cpusPerTask_4_grid_1024.log

module load OpenMPI
mpicc main.c lenia.c gifenc.c orbium.c -O2 -o lenia_MPI -fopenmp -lm -lmpi

# OpenMP environment variables

export OMP_NUM_THREADS=4

mpirun -np $SLURM_NTASKS ./lenia_MPI 1024
