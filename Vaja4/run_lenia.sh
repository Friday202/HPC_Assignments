#!/bin/bash

#SBATCH --reservation=fri
#SBATCH --partition=gpu
#SBATCH --job-name=lenia
#SBATCH --ntasks=2
#SBATCH --time=00:10:00
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --gpus=1
#SBATCH --nodes=2
#SBATCH --output=lenia_out.log


#OMP SETTINGS IF NEEDED
export OMP_PLACES=cores
export OMP_PROC_BIND=TRUE
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

#LOAD MODULES 
module load mpi/openmpi-4.1.3
module load CUDA

#BUILD
make

#RUN
mpirun -np 1 ./lenia.out

rm lenia.o
rm lenia.out
rm orbium.o
rm main.o
# rm lenia_out.log
rm gifenc.o
