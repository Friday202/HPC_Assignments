#!/bin/bash

# On the off chance OMPI_MCA is set to UCX-only, disable that
unset OMPI_MCA_osc

NT=$(lscpu | awk '/per socket:/{print $4}')
NR=2
MAP_BY=socket

mpirun --map-by ${MAP_BY}:PE=$NT -np $NR  \
    -x OMP_NUM_THREADS=$NT -x OMP_PROC_BIND=spread -x OMP_PLACES=cores \
    ./xhpl
