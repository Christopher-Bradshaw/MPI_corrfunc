/* Xi(r) calculates the npairs in 3d spheres
 */
#include <mpi.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>

#include "utils/common.h"
#include "utils/divide_box.h"
#include "./io/io.h"
#include "./io/bins.h"
#include "countpairs.h"
#include "xi_r.h"

int xi_r(xi_r_args *args, results_countpairs *results) {
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double bin_max_r = get_max_r(args->binfile);
    if (bin_max_r == -1) {
        fprintf(stderr, "Reading bin file failed\n");
        return -1;
    }

    double data_region[NUM_FIELDS][2] = {{0}, {0}, {0}};
    double match_region[NUM_FIELDS][2] = {{0}, {0}, {0}};
    get_region_for_rank(
            world_rank, world_size,
            args->boxsize, bin_max_r, args->periodic,
            data_region, match_region);

    double *data[NUM_FIELDS] = {0};
    double *match[NUM_FIELDS] = {0};
    int n_data_points, n_match_points;
    if (read_input_data(
            args->filename1, args->filename2,
            args->format, args->boxsize, args->autocorr,
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
    options.periodic = args->periodic;
    options.boxsize = args->boxsize;
    // If we are doing an autocorrelation overall, each call still needs to be
    // a cross correlation as the match data is different to (a superset of)
    // the data data.
    int cp_autocorr = 0;

    if (countpairs(n_data_points, data[0], data[1], data[2],
                n_match_points, match[0], match[1], match[2],
                args->nthreads, cp_autocorr, args->binfile,
                results, &options, NULL) != 0) {
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

    // Cleanup
    // There is a lot of memory that I am not freeing here...
    for (int i = 0; i < NUM_FIELDS; i++) {
        if (data[i] != match[i]) {
            free(data[i]);
        }
        free(match[i]);
    }

    return 0;
}

int get_xi_r_args(int argc, char **argv, xi_r_args *args) {
    struct option opts[9] = {
        {"filename1", required_argument, NULL, 0},
        {"filename2", required_argument, NULL, 0},
        {"format", required_argument, NULL, 0},
        {"binfile", required_argument, NULL, 0},
        {"boxsize", required_argument, NULL, 0},
        {"nthreads", required_argument, NULL, 0},
        {"autocorr", required_argument, NULL, 0},
        {"periodic", required_argument, NULL, 0},
        {NULL, 0, NULL, 0},
    };

    int opt_index, opt_res;
    int seen = 0;
    while ((opt_res = getopt_long(argc, argv, "", opts, &opt_index)) != -1) {
        if (opt_res != 0) {
            return opt_res;
        }
        switch (opt_index) {
            case 0:
                args->filename1 = optarg;
                seen |= 1 << 0;
                break;
            case 1:
                args->filename2 = optarg;
                seen |= 1 << 1;
                break;
            case 2:
                args->format = optarg[0];
                seen |= 1 << 2;
                break;
            case 3:
                args->binfile = optarg;
                seen |= 1 << 3;
                break;
            case 4:
                sscanf(optarg, "%lf", &args->boxsize);
                seen |= 1 << 4;
                break;
            case 5:
                sscanf(optarg, "%d", &args->nthreads);
                seen |= 1 << 5;
                break;
            case 6:
                sscanf(optarg, "%d", &args->autocorr);
                seen |= 1 << 6;
                break;
            case 7:
                sscanf(optarg, "%d", &args->periodic);
                seen |= 1 << 7;
                break;
        }
    }
    if (seen != 255 && seen != 253) {
        fprintf(stderr, "Only filename2 is optional!\n");
        return -1;
    }
    return 0;
}
