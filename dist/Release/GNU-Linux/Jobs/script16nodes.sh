#!/bin/bash
#SBATCH --nodes 16
#SBATCH --ntasks 16
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 16 ./../netsim_parallel_event 1200 > nodes16.txt

