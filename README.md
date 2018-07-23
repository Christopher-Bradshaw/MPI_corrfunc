# MPI Corrfunc

MPI capable 2 points correlation functions.

## Ways of counting pairs

See [manodeep/Corrfunc cheatsheat](http://corrfunc.readthedocs.io/en/master/all-interfaces.html).

* Xi(r): 3d realspace in spheres with radius r
* Xi(r_p, pi): 3d realspace in cylinders with radius r_p and z distance pi
* Xi(s, mu): ???
* w_p(r_p): 2d projected in circles with radius r_p
* Xi(r): ???
* pN(m): ???

## What this repo does

This doesn't implement any 2pcf finding code. For that we use https://github.com/manodeep/Corrfunc, the best available multicore but single machine code. To run on multiple machines we:

* Parcel out sub-regions to the machines
* Write new io functions to read only the data each rank needs
* Collate the results

## Interface

See the makefile for more examples but you can run it with something like:
```
mpirun -n 4 main xi_r \
    --filename1 ./inputs/ascii_input.txt \
    --format a --binfile ./inputs/bins --boxsize 10 --nthreads 1 \
    --autocorr 1 --periodic 0
```

## Performance

2M ascii input, periodic, autocorrelation:

* 2 nodes, 48 threads: 6.2 - 6.4 seconds
* 4 nodes, 96 threads: 5.9 - 6.2 seconds

This is not good at all... I hope that it is slow from reading the input?

