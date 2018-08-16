#! /bin/bash -l
# Request cluster resources

#SBATCH --nodes 4
#SBATCH --cpus-per-task 16
#SBATCH -t 00:01
#SBATCH -J test_mpi_corrfunc

# SBATCH sets up env vars so that mpirun somehow knows how many jobs to run
# given the number of nodes we have and the number of cpus they have access to
mpirun ./main xi_r \
        --filename1 /homes/cbradshaw/MPI_corrfunc/inputs/long_ascii_input.txt \
        --format a --binfile /homes/cbradshaw/MPI_corrfunc/inputs/bins \
        --boxsize 100 --nthreads 8 \
        --autocorr 1 --periodic 1

# Jupiter has 2x 8 core processors and 64GB of RAM per node
# Run with `sbatch submit_scripts/jupiter.sh`
