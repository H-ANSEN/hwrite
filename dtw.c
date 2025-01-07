#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cglm/vec2.h>

#include "dtw.h"
#include "stroke.h"

void normalize_stroke(Stroke *src, vec2 *dst) {
    unsigned short width = src->max_x - src->min_x;
    unsigned short height = src->max_y - src->min_y;
    unsigned short center_x = src->min_x + (width/2);
    unsigned short center_y = src->min_y + (height/2);
    unsigned short scale = fmax(width, height);

    for (size_t i = 0; i < src->point_count; i++) {
        float new_x = src->points[i][0] - center_x;
        float new_y = src->points[i][1] - center_y;
        if (scale != 0) {
            new_x /= scale;
            new_y /= scale;
        }
        glm_vec2((vec2){new_x, new_y}, dst[i]);
    }
}

DTWMat* dtw_mat_create(Stroke *a, Stroke *b) {
    DTWMat *mat = malloc(sizeof(DTWMat));
    mat->cols = a->point_count + 1;
    mat->rows = b->point_count + 1;
    mat->data = calloc(mat->cols * mat->rows, sizeof(float));

    vec2 *norm_a = malloc(sizeof(vec2) * a->point_count);
    vec2 *norm_b = malloc(sizeof(vec2) * b->point_count);
    normalize_stroke(a, norm_a);
    normalize_stroke(b, norm_b);

    // matrix initialization
    mat->data[0] = 0.0f;
    for (size_t i = 1; i < mat->rows; i++) { mat->data[i*mat->cols] = INFINITY; }
    for (size_t j = 1; j < mat->cols; j++) { mat->data[j] = INFINITY; }

    for (size_t x = 1; x < mat->rows; x++) {
        for (size_t y = 1; y < mat->cols; y++) {
            //vec2 *point_a = &a->points[x - 1];
            //vec2 *point_b = &b->points[y - 1];
            vec2 *point_a = &norm_a[y - 1];
            vec2 *point_b = &norm_b[x - 1];

            const float cost = fabs(glm_vec2_distance(*point_a, *point_b));
            const float insertion = mat->data[x * mat->cols + (y-1)];
            const float deletion = mat->data[(x-1) * mat->cols + y];   
            const float match = mat->data[(x-1) * mat->cols + (y-1)];

            mat->data[x * mat->cols + y] = cost + fminf(fminf(insertion, deletion), match);
        }
    }

    free(norm_a);
    free(norm_b);

    return mat;
}

void dtw_mat_destroy(DTWMat *mat) {
    if (mat) {
        free(mat->data);
        free(mat);
    }
}

void dtw_mat_print(DTWMat *mat) {
    for (size_t x = 0; x < mat->cols; x++) {
        for (size_t y = 0; y < mat->rows; y++) {
            printf("%8.2f ", mat->data[x * mat->cols + y]);
        }
        printf("\n");
    }
}

/*
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

    DTWMat *mat = dtw_mat_create(&a, &b);

    dtw_mat_print(mat);
    for (int i = 0; i < mat->rows * mat->cols; i++) {
        if (mat->data[i] > 5.0 && mat->data[i] < 5.5) {
            printf("i:%d (%8.2f)\n", i, mat->data[i]);
        }
    }

    printf("%ld\n", mat->cols*mat->rows-1);

    dtw_mat_destroy(mat);
}
*/
