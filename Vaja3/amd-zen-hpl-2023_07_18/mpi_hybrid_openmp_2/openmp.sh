#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=openmp
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=128
#SBATCH --nodes=1
#SBATCH --mem=0
#SBATCH --time=10:00:00
#SBATCH --output=mpi_hybrid_openmp/openmp.log

module load OpenMPI/4.1.5-GCC-12.3.0

export UCX_TLS=self, tcp, HPL_RAM_CAP=1.0

unset OMPI_MCA_osc

NT=64
NR=1
MAP_BY=socket

mpirun --map-by ${MAP_BY}:PE=$NT -np $NR -x OMP_NUM_THREADS=$NT -x OMP_PROC_BIND=spread -x OMP_PLACES=cores ./xhpl -p -t -f mpi_hybrid_openmp/openmp.dat