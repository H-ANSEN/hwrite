#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <cglm/vec2.h>

#include "stroke.h"

// Direction points are computed on the 'thin points (t_points)' line, as such
// directions are only pushed when a new thin point is added
static void _push_direction_point(Stroke *s) {
    unsigned short idx0 = s->t_points[s->thin_point_count];
    unsigned short idx1 = s->t_points[s->thin_point_count-1];
    float dx = s->points[idx0][0] - s->points[idx1][0];
    float dy = s->points[idx0][1] - s->points[idx1][1];
    if (fabsf(dx) >= fabsf(dy)) {
        s->directions[s->thin_point_count] = dx >= 0 ? DIRECTION_RIGHT : DIRECTION_LEFT;
    } else {
        s->directions[s->thin_point_count] = dy >= 0 ? DIRECTION_UP : DIRECTION_DOWN;
    }
}

static void _push_thin_point(Stroke *s) {
    if (s->thin_point_count == 0) {
        s->t_points[0] = s->point_count;
        _push_direction_point(s);
        s->thin_point_count++;
    } else {
        unsigned short idx = s->t_points[s->thin_point_count - 1];
        float dx = s->points[s->point_count][0] - s->points[idx][0];
        float dy = s->points[s->point_count][1] - s->points[idx][1];
        if (fabs(dx) >= thinning_factor || fabs(dy) >= thinning_factor) {
            s->t_points[s->thin_point_count] = s->point_count;
            _push_direction_point(s);
            s->thin_point_count++;
        }
    }
}

static void _push_smooth_point(Stroke *s) {
    unsigned short idx;
    if (s->point_count == 0) {
        idx = 1;
        glm_vec2(s->points[0], s->s_points[0]);
    }  else {
        idx = s->point_count;
    }

    vec2 tmp0, tmp1;
    glm_vec2_scale(s->s_points[idx-1], smoothing_factor, tmp0);
    glm_vec2_scale(s->points[idx], 1.0f - smoothing_factor, tmp1);
    glm_vec2_add(tmp0, tmp1, s->s_points[idx]);
}

void draw_points(Stroke *s, SDL_Renderer *r) {
    SDL_RenderLines(r, (SDL_FPoint*)s->points, s->point_count);
}

void draw_smooth_points(Stroke *s, SDL_Renderer *r) {
    SDL_RenderLines(r, (SDL_FPoint*)s->s_points, s->point_count);
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

void draw_direction_arrows(Stroke *s, SDL_Renderer *r) {
    float size = 10;
    float x1, y1, x2, y2, x3, y3;

    for (unsigned short i = 0; i < s->thin_point_count; i++) {
        vec2 *point = &s->points[s->t_points[i]];
        Uint8 direction = s->directions[i];

        switch (direction) {
            case DIRECTION_LEFT:
                x1 = (*point)[0] - size;   y1 = (*point)[1];
                x2 = (*point)[0] + size/2; y2 = (*point)[1] - size/2;
                x3 = (*point)[0] + size/2; y3 = (*point)[1] + size/2;
                break;
            case DIRECTION_RIGHT:
                x1 = (*point)[0] + size;   y1 = (*point)[1];
                x2 = (*point)[0] - size/2; y2 = (*point)[1] - size/2;
                x3 = (*point)[0] - size/2; y3 = (*point)[1] + size/2;
                break;
            case DIRECTION_UP:
                x1 = (*point)[0];          y1 = (*point)[1] + size;
                x2 = (*point)[0] - size/2; y2 = (*point)[1] - size/2;
                x3 = (*point)[0] + size/2; y3 = (*point)[1] - size/2;
                break;
            case DIRECTION_DOWN:
                x1 = (*point)[0];          y1 = (*point)[1] - size;
                x2 = (*point)[0] - size/2; y2 = (*point)[1] + size/2;
                x3 = (*point)[0] + size/2; y3 = (*point)[1] + size/2;
                break;
        }

        SDL_RenderLine(r, x1, y1, x2, y2);
        SDL_RenderLine(r, x3, y3, x1, y1);
    }
}

void push_point(Stroke *s, vec2 point) {
    glm_vec2_copy(point, s->points[s->point_count]);
    _push_smooth_point(s);
    _push_thin_point(s);
    s->point_count++;
}
