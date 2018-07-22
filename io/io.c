#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "io.h"
#include "ascii.h"

// Missing from Corrfunc:
// 1) Option to read floats/doubles
// 2) Option for different number of fields
// 3) Option for non ascii input format
// Why are we doing this?
// We need a parallel reader

int64_t read_input_data(
        const char *filename, const char format, double boxsize,
        double data_region[NUM_FIELDS][2], double match_region[NUM_FIELDS][2],
        int *data_len, int *match_len,
        double **data, double **match) {
    if (format == 'a') {
        return read_ascii_input_data(filename, boxsize,
                data_region, match_region, data_len, match_len, data, match);
    }
    fprintf(stderr, "Only ascii input accepted (format = 'a')");
    return -1;
}

