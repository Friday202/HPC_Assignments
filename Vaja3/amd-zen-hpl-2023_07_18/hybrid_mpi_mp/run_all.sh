sbatch hybrid_mpi_mp/hybrid-p1_q2-NB224-N168448.sh
sleep 2
sbatch hybrid_mpi_mp/hybrid-p2_q1-NB224-N168448.sh
sleep 2

squeue --reservation=fri