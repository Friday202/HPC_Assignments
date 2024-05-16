#!/bin/bash
#SBATCH --reservation=fri
#SBATCH --job-name=16_8
#SBATCH --ntasks=16
#SBATCH --nodes=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=250G
#SBATCH --time=2:00:00
#SBATCH --output=mpi_mp/tasks_16_np_8.log

module load OpenMPI/4.1.5-GCC-12.3.0

MAP_BY=socket

export UCX_TLS=self, tcp
mpirun -np $SLURM_NTASKS --map-by ${MAP_BY}:PE=$SLURM_CPUS_PER_TASK -x OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK -x OMP_PROC_BIND=spread -x OMP_PLACES=cores ./xhpl -p -s 2480 -f mpi_mp/tasks_x_np_y.dat
