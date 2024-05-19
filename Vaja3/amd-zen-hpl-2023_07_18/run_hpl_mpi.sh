#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=hpl-benchmark
#SBATCH --ntasks=2
#SBATCH --nodes=1
#SBATCH --mem=0
#SBATCH --time=2:00:00
#SBATCH --output=hpl_benchmark.log

module load OpenMPI/4.1.5-GCC-12.3.0

export UCX_TLS=self, tcp
mpirun -np $SLURM_NTASKS ./xhpl -p -s 2480 -f HPL.dat
