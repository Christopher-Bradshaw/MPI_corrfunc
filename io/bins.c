#include <stdio.h>
#include <string.h>

#include "bins.h"
#include "io.h"

#define BIN_FIELDS 2

// This assumes that it is a valid binfile. i.e. two cols
double get_max_r(char *binfile) {
    FILE *fp = fopen(binfile, "r");
    if (fp == NULL) {
        return -1;
    }

    char buffer[MAXLINELENGTH];
    char delimiters[]=" ,\t"; // delimiters are white-space, comma and tab

    double tmp, largest_bin = 0;

    while(fgets(buffer, MAXLINELENGTH, fp) != NULL) {
        char *token, *saveptr;
        token = strtok_r(buffer, delimiters, &saveptr);
        sscanf(token, "%lf", &tmp);
        token = strtok_r(NULL, delimiters, &saveptr);
        sscanf(token, "%lf", &tmp);
        if (tmp > largest_bin) {
            largest_bin = tmp;
        }
    }
    return largest_bin;
}
