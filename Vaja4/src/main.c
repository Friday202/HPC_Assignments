#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "lenia.h"

#define N 100
#define NUM_STEPS 100
#define DT 0.1
#define KERNEL_SIZE 26
#define NUM_ORBIUMS 2

//DEMO main. Your main.c will not be compiled into the final app. 

struct orbium_coo orbiums[NUM_ORBIUMS] = {{0, N / 3, 0}, {N / 3, 0, 180}};

int main()
{
    double start = omp_get_wtime();
    // Run the simulation
    // You should provide your own implementation of the function evolve_lenia in order to compete
    // Your code should work for arbitrary world size N, NUM_STEPS, and NUM_ORBIUMS. KERNEL SIZE is fixed to 26
    double *world = evolve_lenia(N, N, NUM_STEPS, DT, KERNEL_SIZE, orbiums, NUM_ORBIUMS);
    double stop = omp_get_wtime();
    printf("Execution time: %.3f\n", stop - start);
    return 0;
}