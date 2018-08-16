#include <stdio.h>
#include <stdarg.h>
#include <mpi.h>

#include "common.h"

void master_log(FILE *stream, const char *format, ...) {
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_rank != MASTER_RANK) {
        return;
    }
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
}
