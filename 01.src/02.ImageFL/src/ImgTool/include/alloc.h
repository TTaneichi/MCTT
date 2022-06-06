/* ---- memory allocation ---- */
#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>
#include <stdio.h>


#define alloc_error_check(p) { \
    if ((p) == NULL) { \
        fprintf(stderr, "Allocation Failure!\n"); \
        exit(1); \
    } \
}

int *alloc_1d_int(int n1);
void free_1d_int(int *i);
double *alloc_1d_double(int n1);
void free_1d_double(double *d);
int **alloc_2d_int(int n1, int n2);
void free_2d_int(int **ii);
double **alloc_2d_double(int n1, int n2);
void free_2d_double(double **dd);
int ***alloc_3d_int(int n1, int n2, int n3);
void free_3d_int(int ***iii);
double ***alloc_3d_double(int n1, int n2, int n3);
void free_3d_double(double ***ddd);

#endif
