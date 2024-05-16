#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=mpi_p2
#SBATCH --ntasks-per-node 128
#SBATCH --nodes=1
#SBATCH --mem=250G
#SBATCH --time=2:00:00
#SBATCH --output=full_mpi/full_MPI-p2_q64-NB224-N168448.log

module load OpenMPI/4.1.5-GCC-12.3.0

export UCX_TLS=self, tcp
mpirun -np 128 ./xhpl -p -s 24800 -f full_mpi/full_MPI-p2_q64-NB224-N168448.dat
