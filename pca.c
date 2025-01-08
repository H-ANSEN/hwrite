#include <cglm/vec2.h>
#include "stroke.h"
#include "mat.h"

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
