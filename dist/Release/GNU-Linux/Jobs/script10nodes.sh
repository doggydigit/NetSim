#!/bin/bash
#SBATCH --nodes 10
#SBATCH --ntasks 10
#SBATCH --cpus-per-task 1
#SBATCH --time 2:00:0
module purge
module load gcc
module load mvapich2

srun -n 10 ./../netsim_parallel_event 1200 > nodes10.txt

