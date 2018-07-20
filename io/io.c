/*
 * Code copied from https://github.com/manodeep/Corrfunc/blob/master/io/io.c
 * and modified
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "io.h"

#define MAXLINELENGTH 1000
#define MEMORY_INCREASE_FAC 1.4 // Unclear what this should be

// Missing from Corrfunc:
// 1) Option to read floats/doubles
// 2) Option for different number of fields
// 3) Option for non ascii input format
// Why are we doing this?
// We need a parallel reader

void cleanup(int idx, char *message, double** data) {
    for (int i = 0; i < idx; i++) {
        free(data[i]);
    }
    fprintf(stderr, "%s", message);
}

int64_t read_input_data(
        const char *filename,
        const char format,
        double limits[NUM_FIELDS][2],
        double **data) {
    if (format != 'a') {
        fprintf(stderr, "Only ascii input accepted (format = 'a')");
        return -1;
    }

    // Open file
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        return -1;
    }

    // Allocate initial memory
    size_t field_size = 8;
    int64_t data_len = 300000;
    int data_idx = 0;

    for(int i = 0; i < NUM_FIELDS; i++) {
        data[i] = malloc(field_size*data_len);
        if (data[i] == NULL) {
            cleanup(i, "Malloc failed\n", data);
            return -1;
        }
    }

    // Read file into that memory
    char buffer[MAXLINELENGTH];
    char delimiters[]=" ,\t"; // delimiters are white-space, comma and tab

    while(fgets(buffer, MAXLINELENGTH, fp) != NULL) {
        char *copy = buffer, *token, *saveptr;
        uint8_t out_of_subbox = 0;

        for(int i=0; i<NUM_FIELDS; i++, copy=NULL) {
            double tmp;
            token = strtok_r(copy, delimiters, &saveptr);
            int nread = sscanf(token, "%lf", &tmp);
            if(nread == 1) {
                // It doesn't matter that things on the boundaries will be included
                // twice because the boundary is actually boundary + buffer. This makes
                // them eligible to match from both sides (not to both sides) which is right.
                if (tmp >= limits[i][0] && tmp <= limits[i][1]) {
                    double *tmp_pos = (double *) data[i];
                    tmp_pos[data_idx] = tmp;
                } else {
                    out_of_subbox = 1;
                    break;
                }
            } else {
                fprintf(stderr, "Not enough fields in row %d", data_idx);
                return -1;
            }
        }
        if (out_of_subbox) {
            printf("No\n");
            continue;
        }

        data_idx++;


        // Resize if necessary
        if (data_idx == data_len) {
            data_len *= MEMORY_INCREASE_FAC;
            assert(data_len > data_idx);

            for (int i = 0; i < NUM_FIELDS; i++) {
                data[i] = realloc(data[i], data_len * field_size);
                if (data[i] == NULL) {
                    cleanup(i, "Realloc fail", data);
                    return -1;
                }
            }
        }

    }
    // Free extra memory
    for (int i = 0; i < NUM_FIELDS; i++) {
        data[i] = realloc(data[i], data_idx * field_size);
        if (data[i] == NULL) {
            cleanup(i, "Final realloc fail", data);
            return -1;
        }
    }
    fclose(fp);
    return data_idx;
}
