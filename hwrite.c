#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cglm/vec2.h>

#include "dtw.h"
#include "stroke.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static bool isMousePressed = false;

static Stroke s = {0};
static Stroke s_prev = {0};

void _compute_similarty() {
    if (s.point_count > 0 && s_prev.point_count > 0) {
        DTWMat *mat = dtw_mat_create(&s, &s_prev);

        SDL_Log("Similarity: %8.2f", mat->data[mat->cols*mat->rows-1]);

        dtw_mat_destroy(mat);
    }

}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("win", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            isMousePressed = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            isMousePressed = false;

            _compute_similarty();

            stroke_clear(&s_prev);
            for (size_t i = 0; i < s.point_count; i++) {
                push_point(&s_prev, s.points[i]);
            }
            stroke_clear(&s);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            if (isMousePressed) {
                vec2 mousePos;
                SDL_GetMouseState(&mousePos[0], &mousePos[1]);
                push_point(&s, mousePos);
            }
            break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    //draw_thin_points(&s, renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
    draw_points(&s, renderer);

    //SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    //draw_smooth_points(&s, renderer);

    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    //draw_direction_arrows(&s, renderer);

    //SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    //draw_corner_markers(&s, renderer);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
