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

2M ascii input, periodic, autocorrelation, 24 threads per node, ignoring IO:

* 1 nodes: 5.4s
* 2 nodes: 3s
* 4 nodes: 2s
* 8 nodes: 1.5s

Doubling the number of nodes gives ~1.5x improvement so this scales roughly as root(n). This is surprising.. I thought it would be much closer to linear.

Ascii IO performance is awful... Reading this file takes 4s.

Tried with 12 threads rather than 24 and is was very slighly worse. 3.2s for 4 nodes.

Running with MPI locally though we get much better results:

* 1 nodes: 4.2
* 2 nodes: 2.1
* 4 nodes: 1.2
* 8 nodes: 0.7

Linear speed up with few nodes, only slightly less than linear as we max out the CPU. But as all these "nodes" are using the same memory, that could be the cause. Even with potential memory contention we get 6x speed up here from 1-8 vs 3x on Edison.
