#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=guzva
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=32
#SBATCH --nodes=1
#SBATCH --mem=250G
#SBATCH --time=4:00:00
#SBATCH --output=test2/test2.log

module load OpenMPI/4.1.5-GCC-12.3.0

MAP_BY=socket

export UCX_TLS=self, tcp
mpirun --map-by ${MAP_BY}:PE=32 -x OMP_NUM_THREADS=32 -x OMP_PLACES=cores ./xhpl -p -s 2480 -f test2/test2.dat
