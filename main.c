#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <mpi.h>

#include "./utils/io.h"
#include "./utils/divide_box.h"
#include "countpairs.h"
#include "defs.h"
#include "./utils/common.h"

void spin_for_gdb() {
#include <unistd.h>
    int i = 0;
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    printf("PID %d on %s ready for attach\n", getpid(), hostname);
    fflush(stdout);
    while (0 == i)
        sleep(5);
    // Connect with gdb /path/to/prog pid
    // Put a breakpoing after this func somewhere
    // Go up a couple of frames and `set var i = 1`
    // Continue
}

int main() {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    printf("Hello world from rank %d out of %d processors\n", world_rank, world_size);

    /* spin_for_gdb(); */

    double boxsize = 10;
    double data_region[NUM_FIELDS][2] = { {0, 10}, {0, 10}, {0, 10} };
    double match_region[NUM_FIELDS][2] = { {0, 10}, {0, 10}, {0, 10} };

    get_region_for_rank(world_rank, world_size, boxsize, 1, data_region, match_region);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    char fname[] = "./inputs/ascii_input.txt";
    char format = 'a';
    char binfile[] = "./inputs/bins";

    int nthreads = 1;
    int autocorr = 0;

    double *data[NUM_FIELDS] = {};
    double *match[NUM_FIELDS] = {};
    int n_data_points, n_match_points;
    if (read_input_data(
            fname, format, boxsize,
            data_region, match_region,
            &n_data_points, &n_match_points,
            data, match) != 0) {
        fprintf(stderr, "Reading input data failed\n");
        return -1;
    }
    double *x1 = data[0], *y1 = data[1], *z1 = data[2];
    double *x2 = match[0], *y2 = match[1], *z2 = match[2];

    printf("%d\n", n_data_points);
    printf("%d\n", n_match_points);

    // get_config_options is ~ get_default_config. We can then modify below
    struct config_options options = get_config_options();
    options.float_type = sizeof(double);
    options.verbose = 0;
    options.periodic = 1;
    options.boxsize = boxsize;

    results_countpairs results;

    int status = countpairs(n_data_points, x1, y1, z1,
                            n_match_points, x2, y2, z2,
                            nthreads,
                            autocorr,
                            binfile,
                            &results,
                            &options, NULL);
    assert(status == 0);

    if (world_rank == MASTER_RANK) {
        uint64_t global_pairs[results.nbin];
        MPI_Reduce(results.npairs, global_pairs, results.nbin, MPI_LONG_LONG, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
        int global_n_data_points;
        MPI_Reduce(&n_data_points, &global_n_data_points, 1, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);

        // Log results
        char outfile[100];
        sprintf(outfile, "./perf/npoints_%d_threads_%d_periodic_%d_rank_ALL", global_n_data_points, nthreads, options.periodic);
        FILE *fd = fopen(outfile, "w");
        for (int i = 0; i < results.nbin; i++) {
            fprintf(fd, "Bin number          : %d\n", i);
            fprintf(fd, "Upper R bound on bin: %lf\n", results.rupp[i]);
            /* fprintf(fd, "Npairs in bin       : %lu\n", results.npairs[i]); */
            fprintf(fd, "Npairs in bin       : %lu\n", global_pairs[i]);
            fprintf(fd, "\n");
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        fprintf(fd, "%lfms\n", (1e9*end_time.tv_sec + end_time.tv_nsec - (
                    1e9*start_time.tv_sec + start_time.tv_nsec)) / 1e6);
        fclose(fd);
    } else {
        MPI_Reduce(results.npairs, NULL, results.nbin, MPI_LONG_LONG, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
        MPI_Reduce(&n_data_points, NULL, 1, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
    }


    MPI_Finalize();
    return 0;
}
