#include <inttypes.h>
#include "../utils/common.h"

#define MAXLINELENGTH 1000
#define MEMORY_INCREASE_FAC 1.4 // Unclear what this should be

int64_t read_input_data(
        const char *filename1, const char *filename2,
        const char format, double boxsize, int autocorr,
        double data_region[NUM_FIELDS][2], double match_region[NUM_FIELDS][2],
        int *data_len, int *match_len,
        double **data, double **match);
