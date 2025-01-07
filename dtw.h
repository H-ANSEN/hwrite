#ifndef __DTW_H__
#define __DTW_H__

#include <stddef.h>

#include "stroke.h"

typedef struct {
    float *data;   /* flattened matrix data */
    size_t rows;
    size_t cols;
} DTWMat;

DTWMat* dtw_mat_create(Stroke *a, Stroke *b);
void dtw_mat_destroy(DTWMat *mat);
void dtw_mat_print(DTWMat *mat);

#endif
