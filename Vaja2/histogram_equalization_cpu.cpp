#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <string>
#include <math.h>  

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define COLOR_CHANNELS 0
#define PIXEL_VALUES 256

void calculate_new_pixel_intensities_cpu(int* histogram_red, int* histogram_green, int* histogram_blue, int width, int height)
{
    int min_red = 0;
    int min_green = 0;
    int min_blue = 0;

    // Find minimum value in each histogram
    for (int i = 0; i < PIXEL_VALUES; ++i)
    {
        if (histogram_red[i] != 0)
        {
            min_red = histogram_red[i];
            break;
        }
    }
    for (int i = 0; i < PIXEL_VALUES; ++i)
    {
        if (histogram_green[i] != 0)
        {
            min_green = histogram_green[i];
            break;
        }
    }
    for (int i = 0; i < PIXEL_VALUES; ++i)
    {
        if (histogram_blue[i] != 0)
        {
            min_blue = histogram_blue[i];
            break;
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
    for (int i = 0; i < width * height * cpp; i += cpp)
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

void map_new_pixel_intensities_cpu(unsigned char* imageIn, const int* histogram_red, const int* histogram_green, const int* histogram_blue, const int width, const int height, const int cpp)
{
    for (int i = 0; i < width * height * cpp; i += cpp)
    {
        imageIn[i] = histogram_red[imageIn[i]];
        imageIn[i + 1] = histogram_green[imageIn[i + 1]];
        imageIn[i + 2] = histogram_blue[imageIn[i + 2]];
    }
}

// example input arguments:
// C:\Users\Name\OneDrive\Namizje\FRI\HPC\CudaRuntime1\img\720x480.png C:\Users\Name\OneDrive\Namizje\FRI\HPC\CudaRuntime1\img\out.png

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

    int runs = 15;
    float averageTime = 0.0f;
    for (int i = 0; i < runs; i++)
    {

        // Load image from file and allocate space for the output image
        int width, height, cpp;
        unsigned char* h_imageIn = stbi_load(szImage_in_name, &width, &height, &cpp, COLOR_CHANNELS);

        /*if (h_imageIn == NULL)
        {
            printf("Error reading loading image %s!\n", szImage_in_name);
            exit(EXIT_FAILURE);
        }
        {
        if (cpp != 3)
            printf("Error: Only RGB images supported!\n");
            exit(EXIT_FAILURE);
        }*/
        // printf("Loaded image %s of size %dx%d.\n", szImage_in_name, width, height);
        const size_t datasize = width * height * cpp * sizeof(unsigned char);    

        int h_histogram_red_cpu[PIXEL_VALUES] = { 0 };
        int h_histogram_green_cpu[PIXEL_VALUES] = { 0 };
        int h_histogram_blue_cpu[PIXEL_VALUES] = { 0 };

        auto start = std::chrono::high_resolution_clock::now();

        // STEP 1: Compute histogram
        calculate_histogram_cpu(h_imageIn, h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu, width, height, cpp);

        // STEP 2: Compute cumulative histogram 
        calculate_cumulative_histogram_cpu(h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu);

        // STEP 3: Compute new pixel intensities
        calculate_new_pixel_intensities_cpu(h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu, width, height);

        // STEP 4: Map new pixel intensities to output image      
        map_new_pixel_intensities_cpu(h_imageIn, h_histogram_red_cpu, h_histogram_green_cpu, h_histogram_blue_cpu, width, height, cpp);

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Print time
        // std::cout << "Time taken: " << duration.count() << " microseconds" << std::endl;
        averageTime += duration.count();

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
            stbi_write_png(szImage_out_name, width, height, cpp, h_imageIn, width * cpp);
        else if (!strcmp(FileType, "jpg"))
            stbi_write_jpg(szImage_out_name, width, height, cpp, h_imageIn, 100);
        else if (!strcmp(FileType, "bmp"))
            stbi_write_bmp(szImage_out_name, width, height, cpp, h_imageIn);
        else
            printf("Error: Unknown image format %s! Only png, bmp, or bmp supported.\n", FileType);

        // Free host memory
        free(h_imageIn);    
    }
    averageTime /= runs;
    averageTime /= 1000.0f;
    printf("Average time cpu (%d runs, %s): %f ms\n", runs, averageTime, szImage_in_name);

    return 0;
}
