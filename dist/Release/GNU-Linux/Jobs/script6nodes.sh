#!/bin/bash
#SBATCH --nodes 6
#SBATCH --ntasks 6
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 6 ./../netsim_parallel_event 1200 > nodes6.txt

