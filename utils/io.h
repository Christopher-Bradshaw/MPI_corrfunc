#define NUM_FIELDS 3
#include <inttypes.h>
int64_t read_input_data(const char *filename, const char format,
        double data_region[NUM_FIELDS][2], double match_region[NUM_FIELDS][2], 
        double **data, double **match);
