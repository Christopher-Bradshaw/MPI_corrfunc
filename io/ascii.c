/*
 * Code copied from https://github.com/manodeep/Corrfunc/blob/master/io/io.c
 * and modified
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "io.h"

uint8_t _in_region(double val, double lower, double upper, double boxsize);
int _initial_alloc(int field_size, int data_len, double **data);
int _realloc(int field_size, int new_len, double **data);

int64_t read_ascii_input_data(
        const char *filename, double boxsize,
        double data_region[NUM_FIELDS][2], double match_region[NUM_FIELDS][2],
        int *data_len, int *match_len,
        double **data, double **match) {

    // Open file
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        return -1;
    }

    // Allocate initial memory
    size_t field_size = 8;
    int64_t data_arr_len = 300000, match_arr_len = 300000;
    int data_idx = 0, match_idx = 0;

    if (_initial_alloc(field_size, data_arr_len, data) != 0) {
        return -1;
    }
    if (_initial_alloc(field_size, match_arr_len, match) != 0) {
        return -1;
    }

    // Read file into that memory
    char buffer[MAXLINELENGTH];
    char delimiters[]=" ,\t"; // delimiters are white-space, comma and tab

    while(fgets(buffer, MAXLINELENGTH, fp) != NULL) {
        char *copy = buffer, *token, *saveptr;
        uint8_t in_data_region = 0, in_match_region = 0;

        for(int i=0; i<NUM_FIELDS; i++, copy=NULL) {
            double tmp;
            token = strtok_r(copy, delimiters, &saveptr);
            int nread = sscanf(token, "%lf", &tmp);
            if(nread == 1) {
                if (_in_region(tmp, data_region[i][0], data_region[i][1], boxsize)) {
                    double *tmp_pos = (double *) data[i];
                    tmp_pos[data_idx] = tmp;
                    in_data_region++;
                    // Write to both, in case future dimensions are not in the data region
                    // but are in the match region.
                    tmp_pos = (double *) match[i];
                    tmp_pos[match_idx] = tmp;
                    in_match_region++;
                } else if (_in_region(tmp, match_region[i][0], match_region[i][1], boxsize)) {
                    double *tmp_pos = (double *) match[i];
                    tmp_pos[match_idx] = tmp;
                    in_match_region++;
                } else {
                    break;
                }
            } else {
                fprintf(stderr, "Not enough fields in row %d", data_idx);
                return -1;
            }
        }
        if (in_data_region == NUM_FIELDS) {
            data_idx++;
        } else if (in_match_region == NUM_FIELDS) {
            match_idx++;
        } else {
            continue;
        }

        // Resize if necessary
        if (data_idx == data_arr_len) {
            data_arr_len *= MEMORY_INCREASE_FAC;
            if (_realloc(field_size, data_arr_len, data) != 0) {
                return -1;
            }
        } else if (match_idx == match_arr_len) {
            match_arr_len *= MEMORY_INCREASE_FAC;
            if (_realloc(field_size, match_arr_len, match) != 0) {
                return -1;
            }
        }
    }
    fclose(fp);

    // Put match after data (as match is a superset of data
    _realloc(field_size, data_idx + match_idx, data);
    for (int i = 0; i < NUM_FIELDS; i++) {
        memcpy(data[i] + data_idx, match[i], match_idx * field_size);
        match[i] = data[i];
    }

    *data_len = data_idx;
    *match_len = data_idx + match_idx;
    return 0;
}

// If * : in region, - : in box, _ : out of box
// 1) ___---l******u---___: check if l <= v <= u (simple case)
// 1) _l_**************_u_: simple case still works if both are outside
// 2) _l_******u------*___: simple case OR v >= (boxsize + l)
// 3) ___*------l******_u_: simple case OR v <= (u - boxsize)
// In all cases the regions are [lower, upper). This comes from the fact that our
// data is [0, boxsize), which I assert here.
uint8_t _in_region(double val, double lower, double upper, double boxsize) {
    assert(val >= 0 && val < boxsize);
    if (lower <= val && val < upper) { // case 1
        return 1;
    } else if (lower < 0 && val >= boxsize + lower) { // special case 2
        return 1;
    } else if (upper >= boxsize && val < upper - boxsize) { // special case 3
        return 1;
    }
    return 0;
}


int _initial_alloc(int field_size, int data_len, double **data) {
    for (int i = 0; i < NUM_FIELDS; i++) {
        data[i] = malloc(field_size*data_len);
        if (data[i] == NULL) {
            fprintf(stderr, "Malloc failed\n");
            return -1;
        }
    }
    return 0;
}

int _realloc(int field_size, int new_len, double **data) {
    for (int i = 0; i < NUM_FIELDS; i++) {
        data[i] = realloc(data[i], new_len * field_size);
        if (data[i] == NULL) {
            fprintf(stderr, "Realloc failed\n");
            return -1;
        }
    }
    return 0;
}
