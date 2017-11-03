#!/bin/bash
#SBATCH --nodes 8
#SBATCH --ntasks 8
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 8 ./../netsim_parallel_event 1200 > nodes8.txt

