#!/bin/bash

#SBATCH --partition=gpu
#SBATCH --reservation=fri
#SBATCH --job-name=code_sample
#SBATCH --gpus=1
#SBATCH --output=output_he.txt
#SBATCH --error=error_he.txt

module load CUDA
nvcc  -diag-suppress 550 -O2 -lm histogram_equalization.cu -o histogram_equalization
# srun  histogram_equalization img/valve.png img_out/valve.png
srun  histogram_equalization img/7680x4320.png img_out/7680x4320.png
