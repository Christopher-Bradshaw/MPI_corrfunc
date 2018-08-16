#define _XOPEN_SOURCE 500
#include <unistd.h>

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <mpi.h>

#include "./io/io.h"
#include "./utils/divide_box.h"
#include "./utils/common.h"

#include "countpairs.h"
#include "defs.h"

#include "xi_r.h"

void _log_results(char *funcname, results_countpairs results, int periodic, int autocorr);
void _spin_for_gdb();
void _say_hello();

int main(int argc, char **argv) {
    /* _spin_for_gdb(); // For debugging */
    MPI_Init(NULL, NULL);

    _say_hello(); // For testing

    if (argc == 1) {
        fprintf(stderr, "Pass your desired corelation func as arg 1. Valid options are:\n"
                "xi_r\n");
        return -1;
    }

    results_countpairs results;
    if (strcmp(argv[1], "xi_r") == 0) {
        xi_r_args args = {};
        if (get_xi_r_args(argc, argv, &args) != 0) {
            fprintf(stderr, "Couldn't read args\n");
            return -1;
        }

        if (xi_r(&args, &results) == -1) {
            fprintf(stderr, "Running xi_r failed\n");
            return -1;
        }
        _log_results("xi_r", results, args.periodic, args.autocorr);
    } else {
        fprintf(stderr, "Bad choice...\n");
        return -1;
    }

    MPI_Finalize();
    return 0;
}

void _log_results(char *funcname, results_countpairs results,
        int periodic, int autocorr) {
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_rank != MASTER_RANK) {
        return;
    }
    char outfile[100];
    sprintf(outfile, "./perf/%s_periodic_%d_autocorr_%d_long", funcname, periodic, autocorr);
    FILE *fd = fopen(outfile, "w");
    for (int i = 0; i < results.nbin; i++) {
        fprintf(fd, "Bin number          : %d\n", i);
        fprintf(fd, "Upper R bound on bin: %lf\n", results.rupp[i]);
        fprintf(fd, "Npairs in bin       : %lu\n", results.npairs[i]);
        fprintf(fd, "\n");
    }
    fclose(fd);
}

void _say_hello() {
    int world_rank, world_size, processor_name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Get_processor_name(processor_name, &processor_name_len);

    printf("Hello from %d of %d on processor %s\n", world_rank, world_size, processor_name);
}


void _spin_for_gdb() {
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
