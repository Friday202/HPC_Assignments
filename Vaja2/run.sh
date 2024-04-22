#!/bin/bash

#SBATCH --partition=gpu
#SBATCH --reservation=fri
#SBATCH --job-name=gpu_hist
#SBATCH --gpus=1
#SBATCH --output=gpu_results.txt
#SBATCH --error=error_he.txt
#SBATCH --time=06:00:00

module load CUDA
nvcc  -diag-suppress 550 -O2 -lm histogram_equalization.cu -o histogram_equalization
# srun  histogram_equalization img/valve.png img_out/valve.png 1024

# USAGE: input_image output_image histBlockSize newPixelBlockSize mapPixelBlockSize
srun histogram_equalization img/720x480.png img_out/720x480.png 992 384 256
srun histogram_equalization img/1024x768.png img_out/1024x768.png 992 512 256
srun histogram_equalization img/1920x1200.png img_out/1920x1080.png 1024 512 256
srun histogram_equalization img/3840x2160.png img_out/3840x2160.png 1024 512 256
srun histogram_equalization img/7680x4320.png img_out/7680x4320.png 1024 512 256