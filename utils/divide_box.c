#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "divide_box.h"
#include "common.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

double _mod(double val, double box_size);

int get_region_for_rank(
        int rank,
        int world_size,
        double box_size,
        double max_r,
        double data_region[NUM_FIELDS][2],
        double match_region[NUM_FIELDS][2]) {

    // We want to maximise the volume to area ratio of each region.
    // The best cuboid for this is a cube. Try keep each side similar sized
    int s1, s2, s3;

    double ideal_per_side = pow(world_size, 1./3);
    s1 = floor(ideal_per_side);

    double remainder = world_size / s1;

    ideal_per_side = pow(remainder, 1./2);

    s2 = floor(ideal_per_side);
    s3 = floor(remainder /= s2);

    if (rank == MASTER_RANK) {
        printf("%d %d %d\n", s1, s2, s3);
        if (s1 * s2 * s3 != world_size) {
            fprintf(stderr, "Only using %d ranks. Consider reducing your allocation"
                    "to that or increasing it to the next 'good' number\n", s1*s2*s3);
        }
    }

    int t1 = rank / (s2 * s3);
    int t2 = (rank - t1 * s2 * s3) / s3;
    int t3 = rank - t1 * s2 * s3 - t2 * s3;

    printf("%d\t%d %d %d\n", rank, t1, t2, t3);

    // These will be within the box
    data_region[0][0] = t1 * box_size/s1;
    data_region[0][1] =(t1+1) * box_size/s1;
    data_region[1][0] = t2 * box_size/s2;
    data_region[1][1] =(t2+1) * box_size/s2;
    data_region[2][0] = t3 * box_size/s3;
    data_region[2][1] =(t3+1) * box_size/s3;

    // These might be outside the box. If we mod them back in the box that would cause issues:
    // e.g. consider if there is only a single division with boxsize 10 and max_r 1.
    // Then the max region is naively -1 - 11. After modding this becomes 9 - 1 and most data
    // is excluded!
    match_region[0][0] = _mod(t1 *     box_size/s1 - max_r, box_size);
    match_region[0][1] = _mod((t1+1) * box_size/s1 + max_r, box_size);
    match_region[1][0] = _mod(t2 *     box_size/s2 - max_r, box_size);
    match_region[1][1] = _mod((t2+1) * box_size/s2 + max_r, box_size);
    match_region[2][0] = _mod(t3 *     box_size/s3 - max_r, box_size);
    match_region[2][1] = _mod((t3+1) * box_size/s3 + max_r, box_size);

    return 0;
}

double _mod(double val, double box_size) {
    if (val >= 0 && val < box_size) {
        return val;
    } else if (val < 0) {
        return val + box_size;
    } else if (val >= box_size) {
        return val - box_size;
    }
    exit(-1);
}

