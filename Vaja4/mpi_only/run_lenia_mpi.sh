#!/bin/sh
#SBATCH --reservation=fri
#SBATCH --job-name=lenia_MPI
#SBATCH --time=00:01:00 
#SBATCH --ntasks=4
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=4
#SBATCH --output=logs/nodes_1_tasks_2_cpusPerTask_32.log

module load OpenMPI
mpicc main.c lenia.c gifenc.c orbium.c -O2 -o lenia_MPI -fopenmp -lm -lmpi

# OpenMP environment variables
export OMP_PLACES=cores
export OMP_PROC_BIND=TRUE
export OMP_NUM_THREADS=$CPUS_PER_TASK_MPI

mpirun -np $SLURM_NTASKS ./lenia_MPI 100

rm lenia_MPI
