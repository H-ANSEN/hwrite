#ifndef __STROKE_H__
#define __STROKE_H__

#include <SDL3/SDL.h>
#include <cglm/vec2.h>

#define MAX_POINT_COUNT 4086

static float smoothing_factor = 0.90f;
static float thinning_factor = 10.0f;

typedef struct {
    unsigned short point_count;
    unsigned short thin_point_count;
    unsigned short t_points[MAX_POINT_COUNT];
    vec2 points[MAX_POINT_COUNT];
} Stroke;

void push_point(Stroke *s, vec2 point);
void draw_thin_points(Stroke *s, SDL_Renderer *r);

#endif /* __STROKE_H__ */
