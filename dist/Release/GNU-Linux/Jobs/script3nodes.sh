#!/bin/bash
#SBATCH --nodes 3
#SBATCH --ntasks 3
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 3 ./../netsim_parallel_event 1200 > nodes3.txt

