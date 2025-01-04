#ifndef __STROKE_H__
#define __STROKE_H__

#include <SDL3/SDL.h>
#include <cglm/vec2.h>

#define MAX_POINT_COUNT 2048
#define DIRECTION_LEFT     0
#define DIRECTION_RIGHT    1
#define DIRECTION_UP       2
#define DIRECTION_DOWN     3

static float smoothing_factor = 0.90f;
static float thinning_factor = 10.0f;

typedef struct {
    unsigned short point_count;
    unsigned short thin_point_count;
    unsigned short t_points[MAX_POINT_COUNT];
    vec2 points[MAX_POINT_COUNT];
    vec2 s_points[MAX_POINT_COUNT];
    Uint8 directions[MAX_POINT_COUNT];
} Stroke;

void push_point(Stroke *s, vec2 point);

void draw_points(Stroke *s, SDL_Renderer *r);
void draw_thin_points(Stroke *s, SDL_Renderer *r);
void draw_smooth_points(Stroke *s, SDL_Renderer *r);
void draw_direction_arrows(Stroke *s, SDL_Renderer *r);

#endif /* __STROKE_H__ */
