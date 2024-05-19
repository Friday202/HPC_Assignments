sbatch mpi_hybrid_openmp_2/openmp.sh
sleep 2
sbatch mpi_hybrid_openmp_2/hybrid.sh
sleep 2
sbatch mpi_hybrid_openmp_2/mpi.sh
sleep 2

squeue --reservation=fri