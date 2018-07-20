#define NUM_FIELDS 3
#include <inttypes.h>
int64_t read_input_data(
        const char *filename, const char format, double limits[NUM_FIELDS][2], double**data);
