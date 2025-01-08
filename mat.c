#include <stddef.h>
#include <stdlib.h>

#include "mat.h"

Mat* mat_create(size_t rows, size_t cols) {
    Mat *mat = malloc(sizeof(Mat));
    mat->rows = rows;
    mat->cols = cols;
    mat->data = calloc(mat->rows * mat->cols, sizeof(float));
    return mat;
}

void mat_free(Mat *mat) {
    if (mat) {
        free(mat->data);
        free(mat);
    }
}

void mat_mean(const Mat *data, float *mean) {
    for (int j = 0; j < data->cols; j++) {
        mean[j] = 0;
        for (int i = 0; i < data->rows; i++) {
            mean[j] += MAT_ELEM(data, i, j);
        }
        mean[j] /= data->rows;
    }
}

void mat_center(Mat *data) {
    float mean[data->cols];
    mat_mean(data, mean);

    for (int i = 0; i < data->rows; i++) {
        for (int j = 0; j < data->cols; j++) {
            MAT_ELEM(data, i, j) -= mean[j];
        }
    }
}

Mat* mat_cov(const Mat *centered_mat) {
    Mat *cov = mat_create(centered_mat->cols, centered_mat->cols);

    for (int i = 0; i < centered_mat->cols; i++) {
        for (int j = 0; j < centered_mat->cols; j++) {
            float sum = 0;
            for (int k = 0; k < centered_mat->rows; k++) {
                sum += MAT_ELEM(centered_mat, k, i) * 
                       MAT_ELEM(centered_mat, k, j);
            }
            MAT_ELEM(cov, i, j) = sum / (centered_mat->rows - 1);
        }
    }

    return cov;
}
