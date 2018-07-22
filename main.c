#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <mpi.h>

#include "./io/io.h"
#include "./utils/divide_box.h"
#include "countpairs.h"
#include "defs.h"
#include "xi_r.h"
#include "./utils/common.h"

void _log_results(char *funcname, results_countpairs results);
void _spin_fog_gdb();

int main(int argc, char **argv) {
    /* _spin_for_gdb(); // For debugging */
    MPI_Init(NULL, NULL);
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    if (argc == 1) {
        fprintf(stderr, "Pass your desired corelation func as arg 1. Valid options are:\n"
                "xi_r\n");
        return -1;
    }

    results_countpairs results;
    if (strcmp(argv[1], "xi_r") == 0) {
        // config (that should probably not be hardcoded...)
        char *filename1 = "./inputs/ascii_input.txt";
        char *filename2 = "./inputs/ascii_input2.txt";
        filename2 = NULL;
        char format = 'a';
        char binfile[] = "./inputs/bins";
        double boxsize = 10;
        int nthreads = 1;
        int autocorr = 1;
        int periodic = 1;
        // end of config
        if (xi_r(filename1, filename2, format, binfile,
                    boxsize, nthreads, autocorr, periodic, &results) == -1) {
            fprintf(stderr, "Running xi_r failed\n");
            return -1;
        }
    } else {
        fprintf(stderr, "Bad choice...\n");
        return -1;
    }
    _log_results(argv[1], results);


    clock_gettime(CLOCK_MONOTONIC, &end_time);
    fprintf(stderr, "%lfms\n", (1e9*end_time.tv_sec + end_time.tv_nsec - (
                1e9*start_time.tv_sec + start_time.tv_nsec)) / 1e6);
    MPI_Finalize();
    return 0;
}

void _log_results(char *funcname, results_countpairs results) {
    // Log results
    char outfile[100];
    sprintf(outfile, "./perf/%s", funcname);
    FILE *fd = fopen(outfile, "w");
    for (int i = 0; i < results.nbin; i++) {
        fprintf(fd, "Bin number          : %d\n", i);
        fprintf(fd, "Upper R bound on bin: %lf\n", results.rupp[i]);
        fprintf(fd, "Npairs in bin       : %lu\n", results.npairs[i]);
        fprintf(fd, "\n");
    }
    fclose(fd);
}

void _spin_for_gdb() {
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
