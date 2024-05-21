#!/bin/sh
#SBATCH --job-name=lenia_sequential
#SBATCH --time=00:01:00 
##SBATCH --reservation=fri
#SBATCH --ntasks=2
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --output=lenia_sequential.log

module load OpenMPI
mpicc lenia_sequential.cu -O2 -o lenia_sequential
mpirun -np $SLURM_NTASKS ./lenia_sequential