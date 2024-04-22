#!/bin/bash

#SBATCH --partition=gpu
#SBATCH --reservation=fri
#SBATCH --job-name=speeeed
#SBATCH --gpus=1
#SBATCH --output=output_he.txt
#SBATCH --error=error_he.txt
#SBATCH --time=06:00:00

module load CUDA
nvcc  -diag-suppress 550 -O2 -lm histogram_equalization_benchmark.cu -o histogram_equalization_benchmark
# srun  histogram_equalization img/valve.png img_out/valve.png 1024

# USAGE: input_image output_image histBlockSize newPixelBlockSize mapPixelBlockSize
srun histogram_equalization_benchmark img/720x480.png img_out/720x480.png 720x480.csv
srun histogram_equalization_benchmark img/1024x768.png img_out/1024x768.png 1024x768.csv
srun histogram_equalization_benchmark img/1920x1200.png img_out/1920x1200.png 1920x1200.csv
srun histogram_equalization_benchmark img/3840x2160.png img_out/3840x2160.png 3840x2160.csv
srun histogram_equalization_benchmark img/7680x4320.png img_out/7680x4320.png 7680x4320.csv 