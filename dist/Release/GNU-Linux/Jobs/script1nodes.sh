#!/bin/bash
#SBATCH --nodes 1
#SBATCH --ntasks 1
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 1 ./../netsim_parallel_event 1200 > nodes1.txt

