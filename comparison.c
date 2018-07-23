#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include "2pcf_plugins/Corrfunc/io/io.h"
#include "countpairs.h"
#include "defs.h"

int main() {
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    char fname1[] = "./inputs/long_ascii_input.txt";
    char fname2[] = "./inputs/long_ascii_input2.txt";
    char format[] = "a";
    char binfile[] = "./inputs/bins";

    // get_config_options is ~ get_default_config. We can then modify below
    struct config_options options = get_config_options();
    options.float_type = sizeof(double);
    options.verbose = 0;
    options.boxsize = 10;
    options.periodic = 0;

    int autocorr = 0;
    int nthreads = 8;
    double *x1=NULL, *y1=NULL, *z1=NULL;
    int npoints1 = read_positions(fname1, format, sizeof(*x1), 3, &x1, &y1, &z1);
    double *x2=x1, *y2=y1, *z2=y1;
    int npoints2 = npoints1;
    if (!autocorr) {
        npoints2 = read_positions(fname2, format, sizeof(*x2), 3, &x2, &y2, &z2);
    }

    results_countpairs results;
    int status = countpairs(npoints1, x1, y1, z1,
                            npoints2, x2, y2, z2,
                            nthreads,
                            autocorr,
                            binfile,
                            &results,
                            &options, NULL);
    assert(status == 0);

    char outfile[100];
    sprintf(outfile, "./perf/cmp_periodic_%d_autocorr_%d_large", options.periodic, autocorr);
    FILE *fd = fopen(outfile, "w");
    for (int i = 0; i < results.nbin; i++) {
        fprintf(fd, "Bin number          : %d\n", i);
        fprintf(fd, "Upper R bound on bin: %lf\n", results.rupp[i]);
        fprintf(fd, "Npairs in bin       : %lu\n", results.npairs[i]);
        fprintf(fd, "\n");
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    printf("%lfms\n", (1e9*end_time.tv_sec + end_time.tv_nsec - (
                1e9*start_time.tv_sec + start_time.tv_nsec)) / 1e6);
    return 0;
}
