#!/bin/sh
#SBATCH --job-name=lenia_MPI
#SBATCH --time=00:01:00 
##SBATCH --reservation=fri
#SBATCH --ntasks=2
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --output=lenia_MPI.log

module load OpenMPI
mpicc main.c lenia.c gifenc.c orbium.c -O2 -o lenia_MPI -fopenmp -lm -lmpi
mpirun -np $SLURM_NTASKS ./lenia_MPI