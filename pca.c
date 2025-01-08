#include <cglm/vec2.h>
#include "stroke.h"

#define MAT_ELEM(m, i, j) ((m)->data[(i)*(m)->cols+(j)])

typedef struct {
    float *data;
    size_t rows;
    size_t cols;
} Mat;

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

void mat_center(Mat *data, const float *mean) {
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

// return the number of changes in direction of a stroke [s]. a change in
// direction is indicated when the same direction occurs twice in succesion and
// that direction is not the same as the previous direction.
static float changes_in_direction(const Stroke *s) {
    float count = 0;
    Uint8 prev = s->directions[0];

    for (size_t i = 2; i < s->point_count; i++) {
        Uint8 dir0 = s->directions[i - 1];
        Uint8 dir1 = s->directions[i];
        if (dir0 == dir1 && dir0 != prev) {
            count++;
            prev = dir0;
        }
    }

    return count;
}

// divide the bounding box of the stroke [s] into a 4x4 grid and compute the
// index of the start and end points of the stroke as an index on the grid. the
// start and end indices are stored in the x and y components of [out_v] 
// respectivly.
static void start_end_sector(const Stroke *s, vec2 *out_v) {
    static const float sector_count = 4.0;
    float sector_size_x = (s->max_x - s->min_x) / sector_count;
    float sector_size_y = (s->max_y - s->min_y) / sector_count;

    int start_x = (int)((s->points[0][0] - s->min_x) / sector_size_x);
    int start_y = (int)((s->points[0][1] - s->min_y) / sector_size_y);
    int end_x = (int)((s->points[s->point_count-1][0] - s->min_x) / sector_size_x);
    int end_y = (int)((s->points[s->point_count-1][1] - s->min_y) / sector_size_y);

    (*out_v)[0] = start_y * sector_count + start_x;
    (*out_v)[0] = end_y * sector_count + end_y;
}

// create a single vector [features] of size [count] whose components represent 
// distinguishable features of a stroke [s]
static void create_feature_vector(const Stroke *s, float *features, int *count) {
    float width = s->max_x - s->min_x;
    float height = s->max_y - s->min_y;
    vec2 start_end_sectors; start_end_sector(s, &start_end_sectors);

    // here are the 'features' we will add more as needed
    float aspect_ratio = width / height;
    float center_x = s->min_x + (width/2);
    float center_y = s->min_y + (height/2);
    float corner_count = s->corner_count;
    float direction_change_count = changes_in_direction(s);
    float start_sector_idx = start_end_sectors[0];
    float end_sector_idx = start_end_sectors[1];

    int idx = 0;
    features[idx++] = aspect_ratio;
    features[idx++] = center_x;
    features[idx++] = center_y;
    features[idx++] = corner_count;
    features[idx++] = direction_change_count;
    features[idx++] = start_sector_idx;
    features[idx++] = end_sector_idx;

    *count = idx;
}
