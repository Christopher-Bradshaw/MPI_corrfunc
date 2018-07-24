#!/bin/bash -l
#SBATCH -p debug
#SBATCH -N 4
#SBATCH -t 00:01
#SBATCH -J test_mpi_corrfunc

srun --ntasks-per-node 1 ./MPI_corrfunc/main xi_r \
        --filename1 $SCRATCH/inputs/large.txt \
        --format a --binfile $SCRATCH/inputs/bins \
        --boxsize 100 --nthreads 24 \
        --autocorr 1 --periodic 1
