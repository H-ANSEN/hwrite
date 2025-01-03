#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <cglm/vec2.h>

#include "stroke.h"

static void _push_thin_point(Stroke *s) {
    if (s->thin_point_count == 0) {
        s->t_points[0] = s->point_count;
        s->thin_point_count++;
    } else {
        unsigned short idx = s->t_points[s->thin_point_count - 1];
        float dx = s->points[s->point_count][0] - s->points[idx][0];
        float dy = s->points[s->point_count][0] - s->points[idx][0];
        if (fabs(dx) >= thinning_factor || fabs(dy) >= thinning_factor) {
            s->t_points[s->thin_point_count] = s->point_count;
            s->thin_point_count++;
        }
    }
}

// Note if the gpu api is used here 't_points' can be used as an index buffer
// into the t_points array allowing drawing of the thin points without directly
// comptuing the array
void draw_thin_points(Stroke *s, SDL_Renderer *r) {
    for (unsigned short i = 1; i < s->thin_point_count; i++) {
        vec2 *a = &s->points[s->t_points[i-1]];
        vec2 *b = &s->points[s->t_points[i]];
        SDL_RenderLine(r, (*a)[0], (*a)[1], (*b)[0], (*b)[1]);
    }
}

void push_point(Stroke *s, vec2 point) {
    glm_vec2_copy(point, s->points[s->point_count]);

    _push_thin_point(s);

    s->point_count++;
}
