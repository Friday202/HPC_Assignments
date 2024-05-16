sbatch mpi_mp/tasks_1_np_128.sh
sleep 2
sbatch mpi_mp/tasks_2_np_64.sh
sleep 2
sbatch mpi_mp/tasks_4_np_32.sh
sleep 2
sbatch mpi_mp/tasks_8_np_16.sh
sleep 2
sbatch mpi_mp/tasks_16_np_8.sh
sleep 2
sbatch mpi_mp/tasks_32_np_4.sh
sleep 2
sbatch mpi_mp/tasks_64_np_2.sh
sleep 2
sbatch mpi_mp/tasks_128_np_1.sh
sleep 2

sbatch mpi_mp/tasks_1_np_64.sh
sleep 2
sbatch mpi_mp/tasks_64_np_1.sh

squeue --reservation=fri