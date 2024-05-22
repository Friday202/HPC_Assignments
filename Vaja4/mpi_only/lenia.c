#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lenia.h"
#include "orbium.h"
#include "gifenc.h"

//#include <omp.h>
#include <mpi.h>
//#include <cuda_runtime.h>
//#include <cuda.h>

// Uncomment to generate gif animation
#define GENERATE_GIF

// For prettier indexing syntax
#define w(r, c) (w[(r) * w_cols + (c)])
#define input(r, c) (input[((r) % rows) * cols + ((c) % cols)])

// Function to calculate Gaussian
inline double gauss(double x, double mu, double sigma)
{
    return exp(-0.5 * pow((x - mu) / sigma, 2));
}

// Function for growth criteria
double growth_lenia(double u)
{
    double mu = 0.15;
    double sigma = 0.015;
    return -1 + 2 * gauss(u, mu, sigma); // Baseline -1, peak +1
}

// Function to generate convolution kernel
double *generate_kernel(double *K, const unsigned int size)
{
    // Construct ring convolution filter
    double mu = 0.5;
    double sigma = 0.15;
    int r = size / 2;
    double sum = 0;
    if (K != NULL)
    {
        for (int y = -r; y < r; y++)
        {
            for (int x = -r; x < r; x++)
            {
                double distance = sqrt((1 + x) * (1 + x) + (1 + y) * (1 + y)) / r;
                K[(y + r) * size + x + r] = gauss(distance, mu, sigma);
                if (distance > 1)
                {
                    K[(y + r) * size + x + r] = 0; // Cut at d=1
                }
                sum += K[(y + r) * size + x + r];
            }
        }
        // Normalize
        for (unsigned int y = 0; y < size; y++)
        {
            for (unsigned int x = 0; x < size; x++)
            {
                K[y * size + x] /= sum;
            }
        }
    }
    return K;
}

// Function to perform convolution on input using kernel w
inline double *convolve2d(double *result, const double *input, const double *w, const unsigned int rows, const unsigned int cols, const unsigned int w_rows, const unsigned int w_cols)
{
    if (result != NULL && input != NULL && w != NULL)
    {
        for (unsigned int i = 0; i < rows; i++)
        {
            for (unsigned int j = 0; j < cols; j++)
            {
                double sum = 0;
                for (int ki = w_rows - 1, kri = 0; ki >= 0; ki--, kri++)
                {
                    for (int kj = w_cols - 1, kcj = 0; kj >= 0; kj--, kcj++)
                    {
                        sum += w(ki, kj) * input((i - w_rows / 2 + rows + kri), (j - w_cols / 2 + cols + kcj));
                    }
                }
                result[i * cols + j] = sum;
            }
        }
    }
    return result;
}

// Function to evolve Lenia
double *evolve_lenia(const unsigned int rows, const unsigned int cols, const unsigned int steps, const double dt, const unsigned int kernel_size, const struct orbium_coo *orbiums, const unsigned int num_orbiums)
{
    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Rank %d, size %d\n", rank, size);   

    // Allocate memory
    double *w = (double *)calloc(kernel_size * kernel_size, sizeof(double));   
    double* world = NULL;
    ge_GIF *gif = NULL; 

    // Rank 0 acts as coordinator and initializes the grid
    if (rank == 0)
    {    

#ifdef GENERATE_GIF
        gif = ge_new_gif(
            "lenia_MPI.gif", /* file name */
            cols, rows,      /* canvas size */
            inferno_pallete, /*pallete*/
            8,               /* palette depth == log2(# of colors) */
            -1,              /* no transparency */
            0                /* infinite loop */
        );
#endif       

        world = (double *)calloc(rows * cols, sizeof(double));
        //double *tmp = (double *)calloc(rows * cols, sizeof(double));

        // Generate convolution kernel
        w=generate_kernel(w,kernel_size);

        // Place orbiums
        for (unsigned int o = 0; o < num_orbiums; o++)
        {
            world = place_orbium(world, rows, cols, orbiums[o].row, orbiums[o].col, orbiums[o].angle);
        }

        
    }

    // Broadcast kernel 
    MPI_Bcast(w, kernel_size * kernel_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calcualte local subgrid size
    unsigned int local_rows = rows / size;
    unsigned int local_cols = cols;
    unsigned int local_size = local_rows * local_cols;

    // Allocate memory for local subgrid
    double *local_world = (double *)calloc(local_size, sizeof(double));
    double *local_tmp = (double *)calloc(local_size, sizeof(double));

    printf("Got to MPI Scatter\n");

    // Scatter world to all processes
    MPI_Scatter(world, local_size, MPI_DOUBLE, local_world, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);    // <- TLE JE PROBLEM

    printf("Got past MPI Scatter\n");
    
    // Lenia Simulation
    for (unsigned int step = 0; step < steps; step++)
    {
        printf("Step %d on rank %d\n", step, rank);
        // Convolution
        //tmp = convolve2d(tmp, world, w, rows, cols, kernel_size, kernel_size);

        // Convolve local subgrid
        local_tmp = convolve2d(local_tmp, local_world, w, local_rows, local_cols, kernel_size, kernel_size);

        // Exchange border rows with neighboirs 
        if (rank > 0)
        {
            MPI_Send(local_tmp, local_cols, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(local_tmp, local_cols, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1)
        {
            MPI_Recv(local_tmp + (local_rows - 1) * local_cols, local_cols, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(local_tmp + (local_rows - 2) * local_cols, local_cols, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }

        // Update local subgrid
        for (unsigned int i = 0; i < local_rows; i++)
        {
            for (unsigned int j = 0; j < local_cols; j++)
            {
                local_world[i * local_cols + j] += dt * growth_lenia(local_tmp[i * local_cols + j]);
                local_world[i * local_cols + j] = fmin(1, fmax(0, local_world[i * local_cols + j])); // Clip between 0 and 1
            }
        }              

#ifdef GENERATE_GIF

        // Gather local subgrid to world
        MPI_Gather(local_world, local_size, MPI_DOUBLE, world, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // Rank 0 updates gif 
        if (rank == 0)
        {
            for (unsigned int i = 0; i < rows; i++)
            {
                for (unsigned int j = 0; j < cols; j++)
                {
                    gif->frame[i * rows + j] = world[i * rows + j] * 255;
                }
            }    
            ge_add_frame(gif, 5);        
        }
#endif
        
        // Evolution
        /*
        for (unsigned int i = 0; i < rows; i++)
        {
            for (unsigned int j = 0; j < cols; j++)
            {
                world[i * rows + j] += dt * growth_lenia(tmp[i * rows + j]);
                world[i * rows + j] = fmin(1, fmax(0, world[i * rows + j])); // Clip between 0 and 1
#ifdef GENERATE_GIF
                gif->frame[i * rows + j] = world[i * rows + j] * 255;
#endif
            }
        }
        */

    }

    // Gather local subgrid to world
    MPI_Gather(local_world, local_size, MPI_DOUBLE, world, local_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);


#ifdef GENERATE_GIF
    if (rank == 0)
    {
        ge_close_gif(gif);        
    }
#endif

    // Free all memory
    if (rank == 0)
    {
        free(w);
        free(world);
        //free(tmp);
    }

    free(local_world);
    free(local_tmp);

    MPI_Finalize();

    return world;
}
