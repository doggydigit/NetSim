#!/bin/bash
#SBATCH --nodes 12
#SBATCH --ntasks 12
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 12 ./../netsim_parallel_event 1200 > nodes12.txt

