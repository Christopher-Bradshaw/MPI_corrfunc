#!/bin/bash -l
#SBATCH -p debug
#SBATCH -N 4
#SBATCH -t 00:01
#SBATCH -J test_mpi_corrfunc

srun --ntasks-per-node 2 main xi_r \
        --filename1 /homes/cbradshaw/MPI_corrfunc/inputs/ascii_input.txt \
        --format a --binfile /homes/cbradshaw/MPI_corrfunc/inputs/bins \
        --boxsize 10 --nthreads 8 \
        --autocorr 1 --periodic 1

# Jupiter has 2x 8 core processors and 64GB of RAM per node
# Run with `sbatch submit_scripts/jupiter.sh`
