#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "2pcf_plugins/Corrfunc/io/io.h"
#include "countpairs.h"
#include "defs.h"

int main() {
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
    options.periodic = 0;
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
    for (int i = 0; i < results.nbin; i++) {
        printf("Bin number          : %d\n", i);
        printf("Upper R bound on bin: %lf\n", results.rupp[i]);
        printf("Npairs in bin       : %lu\n", results.npairs[i]);
        printf("\n");
    }
    return 0;
}
