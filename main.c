#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "2pcf_plugins/Corrfunc/io/io.h"
#include "countpairs.h"
#include "defs.h"

int main() {
    char fname[] = "./inputs/ascii_input.txt";
    char format[] = "a";

    float *x1=NULL, *y1=NULL, *z1=NULL;

    int npoints1 = read_positions(fname, format, sizeof(*x1), 3, &x1, &y1, &z1);

    int nthreads = 1, autocorr = 0;
    char binfile[] = "./inputs/bins";

    results_countpairs results;
    // get_config_options is ~ get_default_config. We can then modify below
    struct config_options options = get_config_options();
    /* printf("verbose: %d\n", options.verbose); */
    /* printf("periodic: %d\n", options.periodic); */
    /* printf("boxsize: %f\n", options.boxsize); */
    /* printf("float type: %zu\n", options.float_type); */
    options.float_type = sizeof(float);
    options.verbose = 0;
    options.periodic = 1;
    options.boxsize = 500;

    int status = countpairs(npoints1, x1, y1, z1,
                            npoints1, x1, y1, z1,
                            nthreads,
                            autocorr,
                            binfile,
                            &results,
                            &options, NULL);
    assert(status == 0);
    for (int i = 0; i < results.nbin; i++) {
        printf("Bin number          : %d\n", i);
        printf("Upper R bound on bin: %lf\n", results.rupp[i]);
        printf("Npairs in bin       : %lu\n", results.npairs[i]);
        printf("\n");
    }
    return 0;
}
