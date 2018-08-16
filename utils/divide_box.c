#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "divide_box.h"
#include "common.h"
#include "errors.h"

int get_region_for_rank(
        int rank,
        int world_size,
        double boxsize,
        double max_r,
        int periodic,
        double data_region[NUM_FIELDS][2],
        double match_region[NUM_FIELDS][2]) {

    // We want to maximise the volume to area ratio of each region.
    // The best cuboid for this is a cube.
    // sx is the number of divisions along dimension x
    int s1, s2, s3;

    double ideal_per_side = pow(world_size, 1./3);
    s1 = floor(ideal_per_side);

    double remainder = world_size / s1;
    ideal_per_side = pow(remainder, 1./2);
    s2 = floor(ideal_per_side);
    s3 = floor(remainder /= s2);

    if (s1 * s2 * s3 != world_size) {
        master_log(stderr, "Only using %d ranks. Consider reducing your allocation "
                "to that or increasing it to the next 'good' number\n", s1*s2*s3);
        // N.B. Don't just remove this! The extra regions will be modded into the box and
        // will be counted!
        return -1;
    }

    // tx is the index along that dimension for this rank
    // t3 changes the quickest
    int t1 = rank / (s2 * s3);
    int t2 = (rank - t1 * s2 * s3) / s3;
    int t3 = rank - t1 * s2 * s3 - t2 * s3;

    // These will be in the range [0, boxsize]. When checking if we are in the region
    // we will check for [lower, upper) which ensures there is no double counting.
    data_region[0][0] = t1     * boxsize/s1;
    data_region[0][1] = (t1+1) * boxsize/s1;
    data_region[1][0] = t2     * boxsize/s2;
    data_region[1][1] = (t2+1) * boxsize/s2;
    data_region[2][0] = t3     * boxsize/s3;
    data_region[2][1] = (t3+1) * boxsize/s3;

    // These might be outside the box. If we mod them back in the box that would cause issues:
    // e.g. consider if there is only a single division with boxsize 10 and max_r 1.
    // Then the max region is naively -1 - 11. After modding this becomes 9 - 1 and most data
    // is excluded!
    match_region[0][0] = t1     * boxsize/s1 - max_r;
    match_region[0][1] = (t1+1) * boxsize/s1 + max_r;
    match_region[1][0] = t2     * boxsize/s2 - max_r;
    match_region[1][1] = (t2+1) * boxsize/s2 + max_r;
    match_region[2][0] = t3     * boxsize/s3 - max_r;
    match_region[2][1] = (t3+1) * boxsize/s3 + max_r;

    // If our box isn't periodic we can move anything outside the box to the edge
    if (!periodic) {
        for (int i = 0; i < NUM_FIELDS; i++) {
            match_region[i][0] = fmax(match_region[i][0], 0);
            match_region[i][1] = fmin(match_region[i][1], boxsize);
        }
    }

    return 0;
}
