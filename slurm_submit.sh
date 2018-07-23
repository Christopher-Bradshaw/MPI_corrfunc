#!/bin/bash -l
#SBATCH -p debug
#SBATCH -N 2
#SBATCH -t 00:01
#SBATCH -J test_mpi_corrfunc

srun -n 48 ./main xi_r \
	--filename1 $SCRATCH/inputs/ascii_input.txt \
	--filename2 $SCRATCH/inputs/ascii_input2.txt \
	--format a --binfile $SCRATCH/inputs/bins \
	--boxsize 10 --nthreads 24 \
	--autocorr 0 --periodic 0
