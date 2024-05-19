#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=guzva
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=128
#SBATCH --nodes=1
#SBATCH --mem=250G
#SBATCH --time=4:00:00
#SBATCH --output=test/test.log

module load OpenMPI/4.1.5-GCC-12.3.0

MAP_BY=socket

export UCX_TLS=self, tcp
mpirun --map-by ${MAP_BY}:PE=64 -np 2 -x OMP_NUM_THREADS=64 -x OMP_PROC_BIND=spread -x OMP_PLACES=cores ./xhpl -p -s 2480 -f test/test.dat
