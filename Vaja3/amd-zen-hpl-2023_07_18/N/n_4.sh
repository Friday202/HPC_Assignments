#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=n_4
#SBATCH --ntasks=4
#SBATCH --nodes=1
#SBATCH --mem=250G
#SBATCH --time=2:00:00
#SBATCH --output=N/n_4.log

module load OpenMPI/4.1.5-GCC-12.3.0

export UCX_TLS=self, tcp
mpirun -np $SLURM_NTASKS ./xhpl -p -s 2480 -f N/n_4.dat
