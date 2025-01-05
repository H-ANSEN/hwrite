#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cglm/vec2.h>

#include "stroke.h"

typedef struct {
    float *data;   /* flattened matrix data */
    size_t rows;
    size_t cols;
} DTWMat;

DTWMat* dtw_mat_create(Stroke *a, Stroke *b) {
    DTWMat *mat = malloc(sizeof(DTWMat));
    mat->cols = a->point_count + 1;
    mat->rows = b->point_count + 1;
    mat->data = aligned_alloc(64, mat->cols * mat->rows * sizeof(float));

    // matrix initialization
    mat->data[0] = 0.0f;
    for (size_t i = 1; i < mat->rows; i++) { mat->data[i*mat->cols] = INFINITY; }
    for (size_t j = 1; j < mat->cols; j++) { mat->data[j] = INFINITY; }

    for (size_t x = 1; x < mat->cols; x++) {
        for (size_t y = 1; y < mat->rows; y++) {
            vec2 *point_a = &a->points[x - 1];
            vec2 *point_b = &b->points[y - 1];

            const float cost = glm_vec2_distance(*point_a, *point_b);
            const float insertion = mat->data[x * mat->cols + (y-1)];
            const float deletion = mat->data[(x-1) * mat->cols + y];   
            const float match = mat->data[(x-1) * mat->cols + (y-1)];

            mat->data[x * mat->cols + y] = cost + fminf(fminf(insertion, deletion), match);
        }
    }

    return mat;
}

void dtw_mat_print(DTWMat *mat) {
    for (size_t x = 0; x < mat->cols; x++) {
        for (size_t y = 0; y < mat->rows; y++) {
            printf("%8.2f ", mat->data[x * mat->cols + y]);
        }
        printf("\n");
    }
}

/* TEST
int main() {
    Stroke a = { 0 };
    push_point(&a, (vec2){0.0f, 0.0f});
    push_point(&a, (vec2){1.0f, 5.0f});
    push_point(&a, (vec2){4.0f, 0.5f});
    push_point(&a, (vec2){5.0f, 1.0f});
    push_point(&a, (vec2){9.0f, 0.5f});
    
    Stroke b = { 0 };
    push_point(&b, (vec2){0.0f, 0.0f});
    push_point(&b, (vec2){1.0f, 1.0f});
    push_point(&b, (vec2){2.0f, 5.0f});
    push_point(&b, (vec2){6.0f, 1.0f});
    push_point(&b, (vec2){9.0f, 0.5f});

    DWTMat *mat = dwt_mat_create(&a, &b);

    dwt_mat_print(mat);

    free(mat->data);
    free(mat);
}
*/
