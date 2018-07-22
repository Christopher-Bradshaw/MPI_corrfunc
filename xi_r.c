/* Xi(r) calculates the npairs in 3d spheres
 */
#include <mpi.h>
#include <stdio.h>
#include <stdint.h>

#include "utils/common.h"
#include "utils/divide_box.h"
#include "./io/io.h"
#include "./io/bins.h"
#include "countpairs.h"

int xi_r(
        char *filename1, char *filename2, char format, char *binfile,
        double boxsize, int nthreads, int autocorr, int periodic,
        results_countpairs *results) {
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double data_region[NUM_FIELDS][2] = {{0}, {0}, {0}};
    double match_region[NUM_FIELDS][2] = {{0}, {0}, {0}};
    double bin_max_r = get_max_r(binfile);
    get_region_for_rank(
            world_rank, world_size,
            boxsize, bin_max_r, periodic,
            data_region, match_region);

    double *data[NUM_FIELDS] = {0};
    double *match[NUM_FIELDS] = {0};
    int n_data_points, n_match_points;
    if (read_input_data(
            filename1, filename2,
            format, boxsize, autocorr,
            data_region, match_region,
            &n_data_points, &n_match_points,
            data, match) != 0) {
        fprintf(stderr, "Reading input data failed\n");
        return -1;
    }

    // get_config_options is ~ get_default_config. We can then modify below
    struct config_options options = get_config_options();
    options.float_type = sizeof(double);
    options.verbose = 0;
    options.periodic = periodic;
    options.boxsize = boxsize;
    // Even if we are doing auto-corr overall, in each call we can't.
    // The match data is in a different region to the data data.
    int cp_autocorr = 0;

    if (countpairs(n_data_points, data[0], data[1], data[2],
                n_match_points, match[0], match[1], match[2],
                nthreads, cp_autocorr, binfile, results, &options, NULL) != 0) {
        fprintf(stderr, "Running countpairs failed\n");
        return -1;
    }

    if (world_rank == MASTER_RANK) {
        uint64_t *global_pairs = malloc(results->nbin * sizeof(uint64_t));
        MPI_Reduce(results->npairs, global_pairs, results->nbin,
                MPI_LONG_LONG, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
        printf("%lu\n", global_pairs[1]);
        int global_n_data_points; // We don't actually do anything with this at the moment
        MPI_Reduce(&n_data_points, &global_n_data_points, 1,
                MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
        results->npairs = global_pairs;
    } else {
        MPI_Reduce(results->npairs, NULL, results->nbin,
                MPI_LONG_LONG, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
        MPI_Reduce(&n_data_points, NULL, 1,
                MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
    }
    return 0;
}
