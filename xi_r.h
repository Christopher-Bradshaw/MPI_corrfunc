#include "countpairs.h"
typedef struct {
    char *filename1;
    char *filename2;
    char format;
    char *binfile;
    double boxsize;
    int nthreads;
    int autocorr;
    int periodic;
} xi_r_args ;

int xi_r(xi_r_args *args, results_countpairs *results);

int get_xi_r_args(int argc, char **argv, xi_r_args *args);
