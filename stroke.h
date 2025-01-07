#ifndef __STROKE_H__
#define __STROKE_H__

#include <SDL3/SDL.h>
#include <cglm/vec2.h>

#define MAX_POINT_COUNT 1024
#define DIRECTION_LEFT     0
#define DIRECTION_RIGHT    1
#define DIRECTION_UP       2
#define DIRECTION_DOWN     3

// CORNER DETECTION
#define DIRECTION_COUNT 16
#define SECTOR_SIZE (360.0f / DIRECTION_COUNT)

static float smoothing_factor = 0.90f;
static float thinning_factor = 10.0f;
static float corner_threshold = 50.0f;

typedef struct {
    unsigned short point_count;
    unsigned short corner_count;
    unsigned short thin_point_count;
    unsigned short min_x, max_x, min_y, max_y;
    unsigned short t_points[MAX_POINT_COUNT]; /* indices to 'thinned points' */
    unsigned short c_points[MAX_POINT_COUNT]; /* indices to 'corner points'  */
    vec2 points[MAX_POINT_COUNT];             /* actual point data */
    vec2 s_points[MAX_POINT_COUNT];           /* point data that has been smoothed */
    Uint8 directions[MAX_POINT_COUNT];        /* cardinal direction at a point */
} Stroke;

void stroke_clear(Stroke *s);
void stroke_push_point(Stroke *s, vec2 point);

void stroke_draw_points(Stroke *s, SDL_Renderer *r);
void stroke_draw_thin_points(Stroke *s, SDL_Renderer *r);
void stroke_draw_smooth_points(Stroke *s, SDL_Renderer *r);
void stroke_draw_direction_arrows(Stroke *s, SDL_Renderer *r);
void stroke_draw_corner_markers(Stroke *s, SDL_Renderer *r);
void stroke_draw_bounding_box(Stroke *s, SDL_Renderer *r);

#endif /* __STROKE_H__ */
