#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=hy_p1
#SBATCH --ntasks-per-node 2
#SBATCH --nodes=1
#SBATCH --mem=250G
#SBATCH --time=2:00:00
#SBATCH --output=hybrid/hybrid-p1_q2-NB224-N168448.log

module load OpenMPI/4.1.5-GCC-12.3.0

export UCX_TLS=self, tcp
mpirun -np 2 ./xhpl -p -s 24800 -f hybrid/hybrid-p1_q2-NB224-N168448.dat
