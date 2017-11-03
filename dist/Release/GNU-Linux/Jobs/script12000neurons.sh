#!/bin/bash
#SBATCH --nodes 4
#SBATCH --ntasks 4
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 4 ./../netsim_parallel_event 12000 > neurons12000.txt

