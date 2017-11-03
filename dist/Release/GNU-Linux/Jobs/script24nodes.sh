#!/bin/bash
#SBATCH --nodes 24
#SBATCH --ntasks 24
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 24 ./../netsim_parallel_event 1200 > nodes24.txt

