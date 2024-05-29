#!/bin/sh
#SBATCH --reservation=fri
#SBATCH --job-name=lenia_MPI
#SBATCH --time=10:00:00 
#SBATCH --ntasks=4
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=32
#SBATCH --output=logs/512/nodes_1_tasks_4_cpusPerTask_16_grid_512.log

module load OpenMPI
mpicc main.c lenia.c gifenc.c orbium.c -O2 -o lenia_MPI -fopenmp -lm -lmpi

# OpenMP environment variables

export OMP_NUM_THREADS=16

mpirun -np $SLURM_NTASKS ./lenia_MPI 512
