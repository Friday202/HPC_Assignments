sbatch full_mpi/full_MPI-p1_q128-NB224-N168448.sh
sleep 2
sbatch full_mpi/full_MPI-p2_q64-NB224-N168448.sh
sleep 2
sbatch full_mpi/full_MPI-p4_q32-NB224-N168448.sh
sleep 2
sbatch full_mpi/full_MPI-p8_q16-NB224-N168448.sh
sleep 2

squeue --reservation=fri