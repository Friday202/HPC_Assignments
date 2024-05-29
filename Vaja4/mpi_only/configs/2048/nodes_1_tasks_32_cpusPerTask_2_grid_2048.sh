#!/bin/sh
#SBATCH --reservation=fri
#SBATCH --job-name=lenia_MPI
#SBATCH --time=10:00:00 
#SBATCH --ntasks=32
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=4
#SBATCH --output=logs/2048/nodes_1_tasks_32_cpusPerTask_2_grid_2048.log

module load OpenMPI
mpicc main.c lenia.c gifenc.c orbium.c -O2 -o lenia_MPI -fopenmp -lm -lmpi

# OpenMP environment variables

export OMP_NUM_THREADS=2

mpirun -np $SLURM_NTASKS ./lenia_MPI 2048
