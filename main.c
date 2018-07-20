#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
/* #include <mpi.h> */

#include "2pcf_plugins/Corrfunc/io/io.h"
#include "countpairs.h"
#include "defs.h"

int main() {
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    char fname[] = "./inputs/ascii_input.txt";
    char format[] = "a";
    char binfile[] = "./inputs/bins";

    int nthreads = 1;
    int autocorr = 0;
    float *x1=NULL, *y1=NULL, *z1=NULL;
    int npoints1 = read_positions(fname, format, sizeof(*x1), 3, &x1, &y1, &z1);

    // get_config_options is ~ get_default_config. We can then modify below
    struct config_options options = get_config_options();
    options.float_type = sizeof(float);
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
    sprintf(outfile, "./perf/npoints_%d_threads_%d_periodic_%d", npoints1, nthreads, options.periodic);
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
    return 0;
}
