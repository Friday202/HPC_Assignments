#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <filesystem>

#include <cuda_runtime.h>
#include <cuda.h>
#include <math.h>   
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

__global__ void calculate_new_pixel_intensities_kernel(int* histogram_red, int* histogram_green, int* histogram_blue, const int width, const int height)
{
    // LOG
    // if (blockIdx.x == 0 && threadIdx.x == 0)
	// {
    //     printf("Executing kernel: Calculate new pixel intensities\n");                  
	// }

    __shared__ int min_red;
    __shared__ int min_green;
    __shared__ int min_blue;

    __syncthreads();  

    // Each block computes its own local minimum values
    if (threadIdx.x < 3) {
        if (threadIdx.x == 0) {
            for (int i = 0; i < PIXEL_VALUES; i++) {
                if (histogram_red[i] != 0) {
                    min_red = histogram_red[i];
                    break;
                }
            }
        }
        else if (threadIdx.x == 1) {
            for (int i = 0; i < PIXEL_VALUES; i++) {
                if (histogram_green[i] != 0) {
                    min_green = histogram_green[i];
                    break;
                }
            }
        }
        else if (threadIdx.x == 2) {
            for (int i = 0; i < PIXEL_VALUES; i++) {
                if (histogram_blue[i] != 0) {
                    min_blue = histogram_blue[i];
                    break;
                }
            }
        }
    }

    __syncthreads();       

    // int total_pixels = width * height;
    int global_thread_id = blockIdx.x * blockDim.x + threadIdx.x;

    // TODO: I don't really understand the loop indexes 
    for (int i = global_thread_id; i < PIXEL_VALUES; i += blockDim.x * gridDim.x)
    {
        histogram_red[i] = ((float)(histogram_red[i] - min_red) / (width * height - min_red)) * (PIXEL_VALUES - 1);
        histogram_green[i] = ((float)(histogram_green[i] - min_green) / (width * height - min_green)) * (PIXEL_VALUES - 1);
        histogram_blue[i] = ((float)(histogram_blue[i] - min_blue) / (width * height - min_blue)) * (PIXEL_VALUES - 1);
    }    
}

__global__ void map_new_pixel_intensities_kernel(const unsigned char* imageIn, unsigned char* imageOut, const int* histogram_red, const int* histogram_green, const int* histogram_blue, const int width, const int height, const int cpp)
{
    // LOG
	// if (blockIdx.x == 0 && threadIdx.x == 0)
	// {
	// 	printf("Executing kernel: Map new pixel intensities\n");
	// }

	int global_thread_id = blockIdx.x * blockDim.x + threadIdx.x;
	for (int i = global_thread_id; i < width * height * cpp; i += blockDim.x * gridDim.x)
	{
		if (i % cpp == 0)
		{
			imageOut[i] = histogram_red[imageIn[i]];
		}
		else if (i % cpp == 1)
		{
			imageOut[i] = histogram_green[imageIn[i]];
		}
		else if (i % cpp == 2)
		{
			imageOut[i] = histogram_blue[imageIn[i]];
		}
	}	
}

__global__ void calculate_cumulative_sum_kernel(int* histogram_red, int* histogram_green, int* histogram_blue) //Algorithm 2 Work-efficient parallel scan algorithm.
{   

    //copy to shared memory (DOES NOT IMPROVE PERFORMANCE, INFACT IT MAKES IT WORSE!)
    // __shared__ int shared_histogram_red[PIXEL_VALUES];
    // __shared__ int shared_histogram_green[PIXEL_VALUES];
    // __shared__ int shared_histogram_blue[PIXEL_VALUES];
    // shared_histogram_red[threadIdx.x] = histogram_red[threadIdx.x];
    // shared_histogram_green[threadIdx.x] = histogram_green[threadIdx.x];
    // shared_histogram_blue[threadIdx.x] = histogram_blue[threadIdx.x];
    // __syncthreads();

    int log_pixel_val = 8;

    for (int k = 0; k < log_pixel_val; k++)
    {   
        int drugi = (threadIdx.x << (k+1)) + ((1 << (k + 1)) - 1);
        if (drugi < PIXEL_VALUES)
        {
            int prvi = (threadIdx.x << (k+1)) + (1 << k) - 1;
            // printf("Thread %d (k=%d) (limit=%f): %d[%d] + %d[%d]\n", threadIdx.x, k, pow(2, log_pixel_val-1-k), prvi, histogram_red[prvi], drugi, histogram_red[drugi]);
            if (blockIdx.x == 0) {
                histogram_red[drugi] = histogram_red[prvi] + histogram_red[drugi];
            } else if (blockIdx.x == 1) {
                histogram_green[drugi] = histogram_green[prvi] + histogram_green[drugi];
            } else if (blockIdx.x == 2) {
                histogram_blue[drugi] = histogram_blue[prvi] + histogram_blue[drugi];
            }
            
        }
        __syncthreads();
    }
    for (int k = log_pixel_val; k > 0; k--)
    {

        int drugi = (threadIdx.x << k) + (1 << k) + (1 << (k-1)) - 1;
        if (drugi < PIXEL_VALUES)
        {
            int prvi = (threadIdx.x << k) + (1 << k) - 1;
            // printf("Thread %d (k=-%d) (limit=%f): %d[%d] + %d[%d]\n", threadIdx.x, k, pow(2, log_pixel_val-1-k), drugi, histogram_red[drugi], prvi, histogram_red[prvi]);
            if (blockIdx.x == 0) {
                histogram_red[drugi] = histogram_red[prvi] + histogram_red[drugi];
            } else if (blockIdx.x == 1) {
                histogram_green[drugi] = histogram_green[prvi] + histogram_green[drugi];
            } else if (blockIdx.x == 2) {
                histogram_blue[drugi] = histogram_blue[prvi] + histogram_blue[drugi];
            }
        }
        __syncthreads();
    }

    //copy back to global memory
    // histogram_red[threadIdx.x] = shared_histogram_red[threadIdx.x];
    // histogram_green[threadIdx.x] = shared_histogram_green[threadIdx.x];
    // histogram_blue[threadIdx.x] = shared_histogram_blue[threadIdx.x];
}

__global__ void calculate_cumulative_sum_simple_kernel(int* histogram_red, int* histogram_green, int* histogram_blue)
{    
    // LOG
    // if (blockIdx.x == 0 && threadIdx.x == 0)
    // {
    //     printf("Executing kernel: Calculate cumulative sum\n");
    // }

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
    // LOG
    // if (blockIdx.x == 0 && threadIdx.x == 0)
	// {
	// 	printf("Executing kernel: Calculate histogram\n");
	// }

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
    // TODO: Possibly add reduction ? 
    for (int i = threadIdx.x; i < PIXEL_VALUES; i += blockDim.x) // blockDim.x always goes between 32 - 1024 
	{
		atomicAdd(&histogram_red[i], partial_histogram_red[i]);
		atomicAdd(&histogram_green[i], partial_histogram_green[i]);
		atomicAdd(&histogram_blue[i], partial_histogram_blue[i]);
	}
    
}

// example input arguments:
// C:\Users\Name\OneDrive\Namizje\FRI\HPC\CudaRuntime1\img\720x480.png C:\Users\Name\OneDrive\Namizje\FRI\HPC\CudaRuntime1\img\out.png
// h_ means host, d_ means device
int main(int argc, char* argv[])
{  
    if (argc < 3)
    {
        printf("USAGE: input_image output_image histBlockSize newPixelBlockSize mapPixelBlockSize\n");
        exit(EXIT_FAILURE);
    }

    char szImage_in_name[255];
    char szImage_out_name[255];

    snprintf(szImage_in_name, 255, "%s", argv[1]);
    snprintf(szImage_out_name, 255, "%s", argv[2]);
    
    // int histBlockSize = atoi(argv[3]);
    // int newPixelBlockSize = atoi(argv[4]);
    // int mapPixelBlockSize = atoi(argv[5]);

    //benchmarking
    FILE *fp; 
    const char *filename = "benchmark.csv";
    fp = fopen(filename, "w");
    fprintf(fp, "histBlockSize, newPixelBlockSize, mapPixelBlockSize, avgHistogramMs, avgNewPixelMs, avgMapPixelMs\n");

    for (int i = 32; i <= 1024; i += 32)
    {
        int histBlockSize = i;
        int newPixelBlockSize = i;
        int mapPixelBlockSize = i;

        float avgHistogramMs = 0;
        float avgNewPixelMs = 0;
        float avgMapPixelMs = 0;

        int runs = 15;
        for (int i = 0; i <= runs; i++){
            //-----------------------------------------
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
            // LOG
            // printf("Loaded image %s of size %dx%d.\n", szImage_in_name, width, height);   
            const size_t datasize = width * height * cpp * sizeof(unsigned char);
            unsigned char* h_imageOut = (unsigned char*)malloc(datasize);

            // Calculate size of histogram
            const size_t histogram_size = PIXEL_VALUES * sizeof(int);  

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

            checkCudaErrors(cudaMemset(d_histogram_red, 0, histogram_size));
            checkCudaErrors(cudaMemset(d_histogram_green, 0, histogram_size));
            checkCudaErrors(cudaMemset(d_histogram_blue, 0, histogram_size));

            // Use CUDA events to measure execution time
            cudaEvent_t start, stop, start_overall, stop_overall;
            cudaEventCreate(&start);
            cudaEventCreate(&stop);
            cudaEventCreate(&start_overall);
            cudaEventCreate(&stop_overall);

            cudaEventRecord(start_overall);

            // Copy input image to device
            // cudaEventRecord(start);
            checkCudaErrors(cudaMemcpy(d_imageIn, h_imageIn, datasize, cudaMemcpyHostToDevice));     

            // float memCpyMs = 0;
            // cudaEventRecord(stop);
            // cudaEventSynchronize(stop);
            // cudaEventElapsedTime(&memCpyMs, start, stop);  

            
            // STEP 1: Compute histogram

            // Setup Thread organization
            int allPixels = width * height * cpp; // Theoretical maximum number of threads
            dim3 blockSizeHist(histBlockSize); // 32 ... 1024 threads per block
            dim3 gridSizeHist((allPixels - 1) / blockSizeHist.x + 1); // can be any size

            cudaEventRecord(start);
            calculate_histogram_kernel <<<gridSizeHist, blockSizeHist >>> (d_imageIn, d_histogram_red, d_histogram_green, d_histogram_blue, width, height, cpp);
            cudaDeviceSynchronize();

            float histogramMs = 0;
            cudaEventRecord(stop);
            cudaEventSynchronize(stop);
            cudaEventElapsedTime(&histogramMs, start, stop);

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
            // calculate_cumulative_sum_simple_kernel <<<gridSize, blockSize >>> (d_histogram_red, d_histogram_green, d_histogram_blue);

            // STEP 2: Optimized cumulative sum computation

            // Setup Thread organization
            dim3 blockSizeCumSum(128); // static
            dim3 gridSizeCumSum(3); // static

            float cumulativeMs = 0;
            cudaEventRecord(start);
            calculate_cumulative_sum_kernel <<<gridSizeCumSum, blockSizeCumSum >>> (d_histogram_red, d_histogram_green, d_histogram_blue);
            cudaDeviceSynchronize();
            cudaEventRecord(stop);
            cudaEventSynchronize(stop);
            cudaEventElapsedTime(&cumulativeMs, start, stop);

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

            // Setup Thread organization
            dim3 blockSizeNewPixel(newPixelBlockSize); // static
            dim3 gridSizeNewPixel((allPixels - 1) / blockSizeNewPixel.x + 1); // static

            float newPixelMs = 0;
            cudaEventRecord(start);
            calculate_new_pixel_intensities_kernel <<<gridSizeNewPixel, blockSizeNewPixel >>> (d_histogram_red, d_histogram_green, d_histogram_blue, width, height);
            cudaDeviceSynchronize();
            cudaEventRecord(stop);
            cudaEventSynchronize(stop);
            cudaEventElapsedTime(&newPixelMs, start, stop);

        #if DEBUG
            calculate_new_pixel_intensities_cpu(h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu, width, height);
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
            // STEP 4: Map new pixel intensities to output image

            // Setup Thread organization
            dim3 blockSizeMapPixel(mapPixelBlockSize); 
            dim3 gridSizeMapPixel((allPixels - 1) / blockSizeMapPixel.x + 1); 

            float mapPixelMs = 0;
            cudaEventRecord(start);
            cudaDeviceSynchronize();
            map_new_pixel_intensities_kernel <<<gridSizeMapPixel, blockSizeMapPixel >>> (d_imageIn, d_imageOut, d_histogram_red, d_histogram_green, d_histogram_blue, width, height, cpp);
            cudaDeviceSynchronize();
            cudaEventRecord(stop);
            cudaEventSynchronize(stop);
            cudaEventElapsedTime(&mapPixelMs, start, stop);

            // STEP 5: Copy output image back to host
            // float copyMs = 0;
            // cudaEventRecord(start);
            checkCudaErrors(cudaMemcpy(h_imageOut, d_imageOut, datasize, cudaMemcpyDeviceToHost));
            getLastCudaError("copy_image() execution failed\n");
            // cudaEventRecord(stop);
            // cudaEventSynchronize(stop);
            // cudaEventElapsedTime(&copyMs, start, stop);

            float overallMs = 0;
            cudaEventRecord(stop_overall);
            cudaEventSynchronize(stop_overall);
            cudaEventElapsedTime(&overallMs, start_overall, stop_overall);
            // printf("%0.5f %0.5f %0.5f %0.5f \n", histogramMs, newPixelMs, mapPixelMs, overallMs);

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
            //-----------------------------------------

            avgHistogramMs += histogramMs;
            avgNewPixelMs += newPixelMs;
            avgMapPixelMs += mapPixelMs;
        }
        avgHistogramMs /= runs;
        avgNewPixelMs /= runs;
        avgMapPixelMs /= runs;
        printf("%d, %d, %d, %0.5f, %0.5f, %0.5f\n", histBlockSize, newPixelBlockSize, mapPixelBlockSize, avgHistogramMs, avgNewPixelMs, avgMapPixelMs);
        fflush(stdout);
        fprintf(fp, "%d, %d, %d, %0.5f, %0.5f, %0.5f\n", histBlockSize, newPixelBlockSize, mapPixelBlockSize, avgHistogramMs, avgNewPixelMs, avgMapPixelMs);
    }
    fclose(fp);
    return 0;
}
