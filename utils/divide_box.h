#include "io.h"

int get_region_for_rank(int rank, int world_size, double box_size, double max_r,
        double data_region[NUM_FIELDS][2], double match_region[NUM_FIELDS][2]);
