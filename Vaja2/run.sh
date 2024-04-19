#!/bin/bash

#SBATCH --partition=gpu
#SBATCH --reservation=fri
#SBATCH --job-name=speeeed
#SBATCH --gpus=1
#SBATCH --output=output_he.txt
#SBATCH --error=error_he.txt
#SBATCH --time=06:00:00

module load CUDA
nvcc  -diag-suppress 550 -O2 -lm histogram_equalization.cu -o histogram_equalization
# srun  histogram_equalization img/valve.png img_out/valve.png 1024

# USAGE: input_image output_image histBlockSize newPixelBlockSize mapPixelBlockSize
srun histogram_equalization img/7680x4320.png img_out/7680x4320.png