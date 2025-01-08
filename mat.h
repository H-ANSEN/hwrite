#ifndef __MAT_H__
#define __MAT_H__

#include <stddef.h>

#define MAT_ELEM(m, i, j) ((m)->data[(i)*(m)->cols+(j)])

typedef struct {
    float *data;
    size_t rows;
    size_t cols;
} Mat;

Mat* mat_create(size_t rows, size_t cols);
void mat_free(Mat *mat);
void mat_mean(const Mat *data, float *mean);
void mat_center(Mat *data);
Mat* mat_cov(const Mat *centered_mat);

#endif /* __MAT_H__ */
