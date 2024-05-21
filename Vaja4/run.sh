#!/bin/sh
#SBATCH --job-name=poskusni_mpi_posel
#SBATCH --time=00:01:00 
##SBATCH --reservation=fri
#SBATCH --ntasks=2
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=2
#SBATCH --output=out_mpi_test.log

module load OpenMPI
mpicc mpi_test.c -O2 -o mpi_test
mpirun -np $SLURM_NTASKS ./mpi_test