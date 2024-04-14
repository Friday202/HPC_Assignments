﻿#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <filesystem>

#include <cuda_runtime.h>
#include <cuda.h>
#include "helper_cuda.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define COLOR_CHANNELS 0
#define DEBUG 0
#define CUDA_DEBUG 0
//#define PIXEL_VALUES (1 << sizeof(unsigned char) * 8)
#define PIXEL_VALUES 256

/* CPU code for checking purposes */
void calculate_new_pixel_intensities_cpu(int* histogram_red, int* histogram_green, int* histogram_blue, int width, int height)
{
    int min_red = 0;
    int min_green = 0;
    int min_blue = 0;
    
    // Find minimum value in each histogram
    for (int i = 0; i < PIXEL_VALUES; ++i)
    {
		if (min_red == 0 && histogram_red[i] != 0)
		{
			min_red = histogram_red[i];			
		}
        if (min_green == 0 && histogram_green[i] != 0)
        {
            min_green = histogram_green[i];           
        }
        if (min_blue == 0 && histogram_blue[i] != 0)
        {
            min_blue = histogram_blue[i];            
        }
    }
    
    for (int i = 0; i < PIXEL_VALUES; ++i)
	{
        histogram_red[i] = ((float)(histogram_red[i] - min_red) / (width * height - min_red)) * (PIXEL_VALUES - 1);
        histogram_green[i] = ((float)(histogram_green[i] - min_green) / (width * height - min_green)) * (PIXEL_VALUES - 1);
        histogram_blue[i] = ((float)(histogram_blue[i] - min_blue) / (width * height - min_blue)) * (PIXEL_VALUES - 1);		
	}    
}

void calculate_histogram_cpu(const unsigned char* image, int* histogram_red, int* histogram_green, int* histogram_blue, const int width, const int height, const int cpp)
{
    for (int i = 0; i < width * height * cpp; i+= cpp)
    {
		histogram_red[image[i]]++;
		histogram_green[image[i + 1]]++;
		histogram_blue[image[i + 2]]++;
	}		
}

void calculate_cumulative_histogram_cpu(int* histogram_red, int* histogram_green, int* histogram_blue)
{	
	for (int i = 1; i < PIXEL_VALUES; i++)
	{
        histogram_red[i] += histogram_red[i - 1];
		histogram_green[i] += histogram_green[i - 1];
		histogram_blue[i] += histogram_blue[i - 1];		
	}
}
/* ~ CPU code for checking purposes */

__global__ void calculate_new_pixel_intensities_kernel(int* histogram_red, int* histogram_green, int* histogram_blue, int width, int height)
{
    if (blockIdx.x == 0 && threadIdx.x == 0)
    {
        printf("Executing kernel: Calculate new pixel intensities\n");
        printf("Width: %d, Height: %d\n", width, height);
        for (int i = 0; i < PIXEL_VALUES; i++)
		{
			printf("Values %d R: %d, G: %d, B: %d\n", i, histogram_red[i], histogram_green[i], histogram_blue[i]);
		}
    }

    int min_red = 0;
    int min_green = 0;
    int min_blue = 0;

    __syncthreads();

    // Find minimum value in each histogram - 3 threads 
    if (threadIdx.x == 0 && blockIdx.x == 0)
    {
        for (int i = 0; i < PIXEL_VALUES; i++)
        {
            if (histogram_red[i] != 0)
            {
                min_red = histogram_red[i];
                break;
            }            
        }
    }
    else if (threadIdx.x == 1 && blockIdx.x == 0)
    {
        for (int i = 0; i < PIXEL_VALUES; i++)
        {
            if (histogram_green[i] != 0)
			{
				min_green = histogram_green[i];
				break;
			}                    
        }
    }
    else if (threadIdx.x == 2 && blockIdx.x == 0)
    {
        for (int i = 0; i < PIXEL_VALUES; i++)
        {
            if (histogram_blue[i] != 0)
			{
				min_blue = histogram_blue[i];
				break;
			}         
        }
    }

    __syncthreads();

    int total_pixels = width * height;

    for (int i = threadIdx.x; i < PIXEL_VALUES; i += blockDim.x)
    {
        histogram_red[i] = ((float)(histogram_red[i] - min_red) / (width * height - min_red)) * (PIXEL_VALUES - 1);
        histogram_green[i] = ((float)(histogram_green[i] - min_green) / (width * height - min_green)) * (PIXEL_VALUES - 1);
        histogram_blue[i] = ((float)(histogram_blue[i] - min_blue) / (width * height - min_blue)) * (PIXEL_VALUES - 1);
    }

    if (blockIdx.x == 0 && threadIdx.x == 0)
    {
        /*printf("Hello\n"); 
        printf("Min red: %d, Min green: %d, Min blue: %d\n", min_red, min_green, min_blue);
        for (int i = 0; i < PIXEL_VALUES; i++)
		{
			printf("Values %d R: %d, G: %d, B: %d\n", i, histogram_red[i], histogram_green[i], histogram_blue[i]);
		}*/
    }
}

__global__ void calculate_cumulative_sum_kernel(int* histogram_red, int* histogram_green, int* histogram_blue)
{
    // TODO: Implement parallel cumulative sum calculation
}

__global__ void calculate_cumulative_sum_simple_kernel(int* histogram_red, int* histogram_green, int* histogram_blue)
{    
    if (blockIdx.x == 0 && threadIdx.x == 0)
    {
        printf("Executing kernel: Calculate cumulative sum\n");
    }

    // Simple solution - only first three threads calculate the cumulative sums
    if (threadIdx.x == 0 && blockIdx.x == 0)
    {        
        for (int i = 1; i < PIXEL_VALUES; i++)
        {
            histogram_red[i] += histogram_red[i - 1];
        }    
    }
    else if (threadIdx.x == 1 && blockIdx.x == 0)
    {
        for (int i = 1; i < PIXEL_VALUES; i++)
        {
			histogram_green[i] += histogram_green[i - 1];
        }
    }
    else if (threadIdx.x == 2 && blockIdx.x == 0)
    {
        for (int i = 1; i < PIXEL_VALUES; i++)
        {
            histogram_blue[i] += histogram_blue[i - 1];
        }
    }
}

__global__ void calculate_histogram_kernel(const unsigned char* imageData, int* histogram_red, int* histogram_green, int* histogram_blue, const int width, const int height, const int cpp)
{ 
    if (blockIdx.x == 0 && threadIdx.x == 0)
	{
		printf("Executing kernel: Calculate histogram\n");
	}

    // Shared memory for partial histograms within block
    __shared__ int partial_histogram_red[PIXEL_VALUES];
    __shared__ int partial_histogram_green[PIXEL_VALUES];
    __shared__ int partial_histogram_blue[PIXEL_VALUES];

    // For each thread in block, parallel set partial histogram to 0
    for (int i = threadIdx.x; i < PIXEL_VALUES; i += blockDim.x) // blockDim.x always goes between 32 - 1024 
	{
        partial_histogram_red[i] = 0;
        partial_histogram_green[i] = 0;
        partial_histogram_blue[i] = 0;
        // Should we zero out the histogram as well? - causes it to not work properly for some reason        
	}    

    __syncthreads();

#if CUDA_DEBUG
    if (threadIdx.x == 0)
    {
        int sum = 0;
        for (int i = 0; i < PIXEL_VALUES; i++)
        {
            sum += partial_histogram[i];
        }
        if (sum != 0)
		{
			printf("Block %d: Partial histogram is not zeroed out\n", blockIdx.x);
            printf("Block %d: Partial histogram sum is %d\n", blockIdx.x, sum);
		}    
        if (blockIdx.x == 0)
        {
            for (int i = 0; i < PIXEL_VALUES; i++)
            {
				printf("First block histogram value %d: %d\n", i, partial_histogram[i]);
			}
		}
	}
    __syncthreads();
#endif    

    // TODO: Improve partial histogram so it doesn't jump around in memory
    /*int elements_per_block = (width * height * cpp) / gridDim.x + 1;
    for (int i = threadIdx.x; i < width * height * cpp; i += blockDim.x)
    {

    }*/

    // Calculate partial histogram, this probably causes a lot of cache misses
    int global_thread_id = blockIdx.x * blockDim.x + threadIdx.x;
    for (int i = global_thread_id; i < width * height * cpp; i += blockDim.x * gridDim.x)
	{
		if (i % cpp == 0)
		{
			atomicAdd(&partial_histogram_red[imageData[i]], 1);
		}
		else if (i % cpp == 1)
		{
			atomicAdd(&partial_histogram_green[imageData[i]], 1);
		}
		else if (i % cpp == 2)
		{
			atomicAdd(&partial_histogram_blue[imageData[i]], 1);
		}
	}   

    __syncthreads();

#if CUDA_DEBUG    
    if (threadIdx.x == 0)
    {
		int sum = 0;
        for (int i = 0; i < PIXEL_VALUES; i++)
        {
			sum += partial_histogram[i];
		}
		printf("Block %d: Partial histogram sum is %d\n", blockIdx.x, sum);
    }
    __syncthreads();
#endif  

    // Add aprtial histograms to global histogram
    for (int i = threadIdx.x; i < PIXEL_VALUES; i += blockDim.x) // blockDim.x always goes between 32 - 1024 
	{
		atomicAdd(&histogram_red[i], partial_histogram_red[i]);
		atomicAdd(&histogram_green[i], partial_histogram_green[i]);
		atomicAdd(&histogram_blue[i], partial_histogram_blue[i]);
	}

    // TODO: Possibly add reduction ? 
	// Add partial histograms to global histogram
	//for (int i = threadIdx.x; i < PIXEL_VALUES; i += blockDim.x) // blockDim.x always goes between 32 - 1024 
	//{
	//	atomicAdd(&histogram[i], partial_histogram[i]);
	//}	
}

// example input arguments:
// C:\Users\Name\OneDrive\Namizje\FRI\HPC\CudaRuntime1\img\720x480.png C:\Users\Name\OneDrive\Namizje\FRI\HPC\CudaRuntime1\img\out.png
// h_ means host, d_ means device
int main(int argc, char* argv[])
{  
    if (argc < 3)
    {
        printf("USAGE: input_image output_image\n");
        exit(EXIT_FAILURE);
    }

    char szImage_in_name[255];
    char szImage_out_name[255];

    snprintf(szImage_in_name, 255, "%s", argv[1]);
    snprintf(szImage_out_name, 255, "%s", argv[2]);

    // Load image from file and allocate space for the output image
    int width, height, cpp;
    unsigned char* h_imageIn = stbi_load(szImage_in_name, &width, &height, &cpp, COLOR_CHANNELS);

    if (h_imageIn == NULL)
    {
        printf("Error reading loading image %s!\n", szImage_in_name);
        exit(EXIT_FAILURE);
    }
    if (cpp != 3)
    {
		printf("Error: Only RGB images supported!\n");
		exit(EXIT_FAILURE);
	}
    printf("Loaded image %s of size %dx%d.\n", szImage_in_name, width, height);   
    const size_t datasize = width * height * cpp * sizeof(unsigned char);
    unsigned char* h_imageOut = (unsigned char*)malloc(datasize);

    // Calculate size of histogram
    const size_t histogram_size = PIXEL_VALUES * sizeof(int);  

    // Setup Thread organization
    int allPixels = width * height * cpp; // Theoretical maximum number of threads
    dim3 blockSize(256); // 32 ... 1024 threads per block
    dim3 gridSize((allPixels - 1) / blockSize.x + 1); // can be any size

    // Device memory pointers
    unsigned char* d_imageIn;
    unsigned char* d_imageOut;

    int* d_histogram_red;
    int* d_histogram_green;
    int* d_histogram_blue;

    // Allocate memory on the device
    checkCudaErrors(cudaMalloc(&d_imageIn, datasize));
    checkCudaErrors(cudaMalloc(&d_imageOut, datasize));

    checkCudaErrors(cudaMalloc(&d_histogram_red, histogram_size));
    checkCudaErrors(cudaMalloc(&d_histogram_green, histogram_size));
    checkCudaErrors(cudaMalloc(&d_histogram_blue, histogram_size));

    // Use CUDA events to measure execution time
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // Copy input image to device
    cudaEventRecord(start);
    checkCudaErrors(cudaMemcpy(d_imageIn, h_imageIn, datasize, cudaMemcpyHostToDevice));       

    // STEP 1: Compute histogram
    calculate_histogram_kernel <<<gridSize, blockSize >>> (d_imageIn, d_histogram_red, d_histogram_green, d_histogram_blue, width, height, cpp);

#if DEBUG
    int h_histogram_red[PIXEL_VALUES] = { 0 };
    int h_histogram_green[PIXEL_VALUES] = { 0 };
    int h_histogram_blue[PIXEL_VALUES] = { 0 };

    int h_histogram_red_cpu[PIXEL_VALUES] = { 0 };    
    int h_histogram_green_cpu[PIXEL_VALUES] = { 0 };
    int h_histogram_blue_cpu[PIXEL_VALUES] = { 0 };

    calculate_histogram_cpu(h_imageIn, h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu, width, height, cpp);
    checkCudaErrors(cudaMemcpy(h_histogram_red, d_histogram_red, histogram_size, cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(h_histogram_green, d_histogram_green, histogram_size, cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(h_histogram_blue, d_histogram_blue, histogram_size, cudaMemcpyDeviceToHost));

    for (int i = 0; i < PIXEL_VALUES; i++)
    {
        printf("Values %d R: %d - %d, G: %d - %d, B: %d - %d\n", i, h_histogram_red_cpu[i], h_histogram_red[i], h_histogram_green_cpu[i], h_histogram_green[i], h_histogram_blue_cpu[i], h_histogram_blue[i]);
        assert(h_histogram_red[i] == h_histogram_red_cpu[i]);
        assert(h_histogram_green[i] == h_histogram_green_cpu[i]);
        assert(h_histogram_blue[i] == h_histogram_blue_cpu[i]);
    }
#endif

    // STEP 2: Compute cumulative sum 
    cudaDeviceSynchronize();
    calculate_cumulative_sum_simple_kernel <<<gridSize, blockSize >>> (d_histogram_red, d_histogram_green, d_histogram_blue);

#if DEBUG
    calculate_cumulative_histogram_cpu(h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu);
    checkCudaErrors(cudaMemcpy(h_histogram_red, d_histogram_red, histogram_size, cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(h_histogram_green, d_histogram_green, histogram_size, cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(h_histogram_blue, d_histogram_blue, histogram_size, cudaMemcpyDeviceToHost));
    for (int i = 0; i < PIXEL_VALUES; i++)
    {
        printf("Values %d R: %d - %d, G: %d - %d, B: %d - %d\n", i, h_histogram_red_cpu[i], h_histogram_red[i], h_histogram_green_cpu[i], h_histogram_green[i], h_histogram_blue_cpu[i], h_histogram_blue[i]);
        assert(h_histogram_red[i] == h_histogram_red_cpu[i]);
        assert(h_histogram_green[i] == h_histogram_green_cpu[i]);
        assert(h_histogram_blue[i] == h_histogram_blue_cpu[i]);
	}
#endif   

    // STEP 3: Compute new pixel intensities
    cudaDeviceSynchronize();
    calculate_new_pixel_intensities_kernel <<<gridSize, blockSize >>> (d_histogram_red, d_histogram_green, d_histogram_blue, width, height);

#if DEBUG
    calculate_new_pixel_intensities_cpu(h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu, width, height);
    checkCudaErrors(cudaMemcpy(h_histogram_red, d_histogram_red, histogram_size, cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(h_histogram_green, d_histogram_green, histogram_size, cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(h_histogram_blue, d_histogram_blue, histogram_size, cudaMemcpyDeviceToHost));
    for (int i = 0; i < PIXEL_VALUES; i++)
    {
        printf("Values %d R: %d - %d, G: %d - %d, B: %d - %d\n", i, h_histogram_red_cpu[i], h_histogram_red[i], h_histogram_green_cpu[i], h_histogram_green[i], h_histogram_blue_cpu[i], h_histogram_blue[i]);
        //assert(h_histogram_red[i] == h_histogram_red_cpu[i]);
        //assert(h_histogram_green[i] == h_histogram_green_cpu[i]);
        //assert(h_histogram_blue[i] == h_histogram_blue_cpu[i]);
    }
#endif
    // STEP 4: Map new pixel intensities to output image
    cudaDeviceSynchronize();
    
    // STEP 5: Copy output image back to host
    checkCudaErrors(cudaMemcpy(h_imageOut, d_imageOut, datasize, cudaMemcpyDeviceToHost));
    
    getLastCudaError("copy_image() execution failed\n");
    cudaEventRecord(stop);

    cudaEventSynchronize(stop);

    // Print time
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);
    printf("Kernel Execution time is: %0.3f milliseconds \n", milliseconds);

    // Write the output file
    char szImage_out_name_temp[255];
    strncpy(szImage_out_name_temp, szImage_out_name, 255);
    char* token = strtok(szImage_out_name_temp, ".");
    char* FileType = NULL;
    while (token != NULL)
    {
        FileType = token;
        token = strtok(NULL, ".");
    }

    if (!strcmp(FileType, "png"))
        stbi_write_png(szImage_out_name, width, height, cpp, h_imageOut, width * cpp);
    else if (!strcmp(FileType, "jpg"))
        stbi_write_jpg(szImage_out_name, width, height, cpp, h_imageOut, 100);
    else if (!strcmp(FileType, "bmp"))
        stbi_write_bmp(szImage_out_name, width, height, cpp, h_imageOut);
    else
        printf("Error: Unknown image format %s! Only png, bmp, or bmp supported.\n", FileType);

    // Free device memory
    checkCudaErrors(cudaFree(d_imageIn));
    checkCudaErrors(cudaFree(d_imageOut));
    checkCudaErrors(cudaFree(d_histogram_red));
    checkCudaErrors(cudaFree(d_histogram_green));
    checkCudaErrors(cudaFree(d_histogram_blue));

    // Clean-up events
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    // Free host memory
    free(h_imageIn);
    free(h_imageOut);   

    return 0;
}
