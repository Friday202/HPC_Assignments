sbatch mpi_hybrid_openmp/openmp.sh
sleep 2
sbatch mpi_hybrid_openmp/hybrid.sh
sleep 2
sbatch mpi_hybrid_openmp/mpi.sh
sleep 2

squeue --reservation=fri