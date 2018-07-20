#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
/* #include <mpi.h> */

#include "./io/io.h"
#include "countpairs.h"
#include "defs.h"

int main() {
    /* int world_size; */
    int world_rank = 0;
    /* MPI_Init(NULL, NULL); */
    /* MPI_Comm_size(MPI_COMM_WORLD, &world_size); */
    /* MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); */
    /* // Print off a hello world message */
    /* printf("Hello world from rank %d out of %d processors\n", world_rank, world_size); */
    printf("In main\n");

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    char fname[] = "./inputs/ascii_input.txt";
    char format = 'a';
    char binfile[] = "./inputs/bins";

    int nthreads = 1;
    int autocorr = 0;
    /* float *x1=NULL, *y1=NULL, *z1=NULL; */
    /* int npoints1 = read_positions(fname, format, sizeof(*x1), 3, &x1, &y1, &z1); */
    double *data[NUM_FIELDS] = {};
    double limits[NUM_FIELDS][2] = { {0, 10}, {0, 10}, {0, 10} };
    int npoints1 = read_input_data(fname, format, limits, data);
    double *x1 = data[0], *y1 = data[1], *z1 = data[2];

    printf("%d\n", npoints1);

    // get_config_options is ~ get_default_config. We can then modify below
    struct config_options options = get_config_options();
    options.float_type = sizeof(double);
    options.verbose = 0;
    options.periodic = 1;
    options.boxsize = 10;

    results_countpairs results;

    int status = countpairs(npoints1, x1, y1, z1,
                            npoints1, x1, y1, z1,
                            nthreads,
                            autocorr,
                            binfile,
                            &results,
                            &options, NULL);
    assert(status == 0);


    char outfile[100];
    sprintf(outfile, "./perf/npoints_%d_threads_%d_periodic_%d_rank_%d", npoints1, nthreads, options.periodic, world_rank);
    FILE *fd = fopen(outfile, "w");
    for (int i = 0; i < results.nbin; i++) {
        fprintf(fd, "Bin number          : %d\n", i);
        fprintf(fd, "Upper R bound on bin: %lf\n", results.rupp[i]);
        fprintf(fd, "Npairs in bin       : %lu\n", results.npairs[i]);
        fprintf(fd, "\n");
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    fprintf(fd, "%lfms\n", (1e9*end_time.tv_sec + end_time.tv_nsec - (
                1e9*start_time.tv_sec + start_time.tv_nsec)) / 1e6);

    /* MPI_Finalize(); */
    return 0;
}
