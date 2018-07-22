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
        const char *filename1, const char *filename2,
        const char format, double boxsize, int autocorr,
        double data_region[NUM_FIELDS][2], double match_region[NUM_FIELDS][2],
        int *data_len, int *match_len,
        double **data, double **match) {

    if (autocorr && filename2 != NULL) {
        fprintf(stderr, "If doing an autocorrelation, only pass a single data file\n");
        return -1;
    }

    if (format == 'a') {
        if (autocorr) {
            return read_ascii_autocorr(filename1, boxsize,
                    data_region, match_region, data_len, match_len, data, match);
        } else {
            return (read_ascii(filename1, boxsize, data_region, data_len, data) ||
                    read_ascii(filename2, boxsize, match_region, match_len, match));
        }
    }
    fprintf(stderr, "Only ascii input accepted (format = 'a')");
    return -1;
}

