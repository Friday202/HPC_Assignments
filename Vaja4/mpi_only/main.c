#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "lenia.h"

#include "mpi.h"

#define NUM_STEPS 50
#define DT 0.1
#define KERNEL_SIZE 26
#define NUM_ORBIUMS 2


int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        printf("Invalid value for N\n");
        return 1;
    }

    // printf("Running Lenia simulation with N = %d\n", N);

    double avgTime = 0;
    int num_iterations = 2;
    MPI_Init(NULL, NULL);

    for (int i = 0; i < num_iterations; i++) {
        // Orbiums
        struct orbium_coo orbiums[NUM_ORBIUMS] = {{0, N / 3, 0}, {N / 3, 0, 180}};

        double start = omp_get_wtime();
        // Run the simulation
        double *world = evolve_lenia(N, N, NUM_STEPS, DT, KERNEL_SIZE, orbiums, NUM_ORBIUMS);
        double stop = omp_get_wtime();
        avgTime += stop - start;
    }
    MPI_Finalize();

    avgTime /= num_iterations;
    
    printf("Execution time: %.3f\n", avgTime);

    return 0;
}
