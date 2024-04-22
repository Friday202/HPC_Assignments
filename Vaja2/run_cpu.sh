#!/bin/bash
#SBATCH --job-name=cpu_hist
#SBATCH --partition=all
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --output=cpu_results.txt
#SBATCH --time=05:00:00
#SBATCH --mem=64GB

g++ -std=c++17 histogram_equalization_cpu.cpp -o histogram_equalization_cpu

srun ./histogram_equalization_cpu img/720x480.png img_out/720x480.png
srun ./histogram_equalization_cpu img/1024x768.png img_out/1024x768.png
srun ./histogram_equalization_cpu img/1920x1200.png img_out/1920x1080.png
srun ./histogram_equalization_cpu img/3840x2160.png img_out/3840x2160.png
srun ./histogram_equalization_cpu img/7680x4320.png img_out/7680x4320.png