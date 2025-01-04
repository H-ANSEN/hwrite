#include <SDL3/SDL.h>
#include <math.h>
#include <cglm/vec2.h>

#include "stroke.h"

static void _push_thin_point(Stroke *s);
static void _push_smooth_point(Stroke *s);
static void _push_corner_point(Stroke *s);

void push_point(Stroke *s, vec2 point) {
    glm_vec2_copy(point, s->points[s->point_count]);
    _push_smooth_point(s);
    _push_thin_point(s);
    _push_corner_point(s);
    s->point_count++;
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

void draw_corner_markers(Stroke *s, SDL_Renderer *r) {
    for (unsigned short i = 0; i < s->corner_count; i++) {
        unsigned short idx = s->c_points[i];
        SDL_RenderRect(r, &(SDL_FRect){
                .x = s->points[s->t_points[idx]][0] - 5,
                .y = s->points[s->t_points[idx]][1] - 5,
                .w = 10,
                .h = 10
        });
    }
}


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
        /*
        float dx = s->s_points[s->point_count][0] - s->s_points[idx][0];
        float dy = s->s_points[s->point_count][1] - s->s_points[idx][1];
        */
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

static float _corner_angle(vec2 p1, vec2 p2, vec2 p3) {
    vec2 delta_vec_a = { p2[0] - p1[0], p2[1] - p1[1] };
    vec2 delta_vec_b = { p3[0] - p2[0], p3[1] - p2[1] };

    float dot = glm_vec2_dot(delta_vec_a, delta_vec_b);
    float mag_a = glm_vec2_norm(delta_vec_a);
    float mag_b = glm_vec2_norm(delta_vec_b);

    float angle = acos(dot / (mag_a * mag_b));
    return angle * 180.0f / GLM_PIf;
}

static inline bool _same_direction(vec2 p1, vec2 p2, vec2 p3) {
    return _corner_angle(p1, p2, p3) <= SECTOR_SIZE;
}

static void _push_corner_point(Stroke *s) {
    if (s->thin_point_count < 5) {
        return;
    }

    vec2 *p0 = &s->points[s->t_points[s->thin_point_count-5]];
    vec2 *p1 = &s->points[s->t_points[s->thin_point_count-4]];
    vec2 *p2 = &s->points[s->t_points[s->thin_point_count-3]];
    vec2 *p3 = &s->points[s->t_points[s->thin_point_count-2]];
    vec2 *p4 = &s->points[s->t_points[s->thin_point_count-1]];

    if (!_same_direction(*p0, *p1, *p2)) {
        return;
    }

    float corner_angle = _corner_angle(*p1, *p2, *p3);
    if (corner_angle < corner_threshold) {
        return;
    }

    if (_same_direction(*p2, *p3, *p4) || 
            _same_direction(*p3, *p4, s->points[s->t_points[s->thin_point_count]]))
    {
        s->c_points[s->corner_count] = s->thin_point_count - 3;
        s->corner_count++;
    }
}
