#include <stdio.h>
#include <stdint.h>

#include "2pcf_plugins/Corrfunc/io/io.h"
#include "countpairs.h"
#include "defs.h"

int main() {
    char fname[] = "./inputs/ascii_input.txt";
    char format[] = "ascci";

    double *x1=NULL, *y1=NULL, *z1=NULL;

    int npoints = read_positions(fname, format, sizeof(x1), 3, &x1, &y1, &z1);
    printf("%d\n", npoints);

    int nthreads = 1, autocorr = 0;
    char binfile[] = "./inputs/bins";

    results_countpairs results;
    // get_config_options is ~ get_default_config. We can then modify below
    struct config_options options = get_config_options();
    /* printf("verbose: %d\n", options.verbose); */
    /* printf("periodic: %d\n", options.periodic); */
    /* printf("boxsize: %f\n", options.boxsize); */
    /* printf("float type: %zu\n", options.float_type); */
    options.verbose = 0;
    options.periodic = 1;
    options.boxsize = 500;

    int status = countpairs(npoints, x1, y1, z1,
                            npoints, x1, y1, z1,
                            nthreads,
                            autocorr,
                            binfile,
                            &results,
                            &options, NULL);
    printf("%d\n", status);
    printf("Npairs: %lu\n", *results.npairs);
    printf("Nbin: %u\n", results.nbin);
    printf("Rupp: %lf\n", results.rupp[0]);
    printf("Rpavg: %lf\n", results.rpavg[0]);
    printf("Wavg: %lf\n", results.weightavg[0]);


  /* typedef struct{ */
  /*   uint64_t *npairs; */
  /*   double *rupp; */
  /*   double *rpavg; */
  /*   double *weightavg; */
  /*   int nbin; */
  /* } results_countpairs; */

    return 0;
}
