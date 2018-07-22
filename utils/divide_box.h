#include "io.h"

int get_region_for_rank(int rank, int world_size, double boxsize, double max_r,
        double data_region[NUM_FIELDS][2], double match_region[NUM_FIELDS][2]);
