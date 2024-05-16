#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=hpl-parameter-search
#SBATCH --ntasks=4
#SBATCH --nodes=1
#SBATCH --mem=0
#SBATCH --time=2:00:00
#SBATCH --output=hpl_benchmark.log

module load OpenMPI/4.1.5-GCC-12.3.0

export UCX_TLS=self, tcp
mpirun -np $SLURM_NTASKS --map-by ${MAP_BY}:PE=$NT -np $NR -x OMP_NUM_THREADS=$NT -x OMP_PROC_BIND=spread -x OMP_PLACES=cores ./xhpl -p -s 2480

