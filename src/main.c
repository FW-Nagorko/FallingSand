#include <SDL3/SDL.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "game.h"
#include "rendering.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    GameState game;
    SDL_Time last_update;
    SDL_Texture *game_tex;
} SDL_AppState;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_AppState *state = (SDL_AppState *) SDL_calloc(1, sizeof(SDL_AppState));
    if (!state) {
        SDL_Log("Failed to allocate app state: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        SDL_free(state);
        return SDL_APP_FAILURE;
    }
    if (!SDL_CreateWindowAndRenderer("Falling Sand", 800, 600, 0, &state->window,
                                     &state->renderer)) {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        SDL_Quit();
        SDL_free(state);
        return SDL_APP_FAILURE;
    }

    init_game(&state->game, 50, 50);

    state->game_tex =
        SDL_CreateTexture(state->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                          state->game.width, state->game.height);
    if (!state->game_tex) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        free_game(&state->game);
        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
        SDL_Quit();
        SDL_free(state);
        return SDL_APP_FAILURE;
    }

    SDL_SetTextureScaleMode(state->game_tex, SDL_SCALEMODE_NEAREST);

    set_pixel(&state->game, 25, 25, SAND);
    set_pixel(&state->game, 26, 25, WATER);

    for (int i = 10; i < 40; i++)
        set_pixel(&state->game, i, 40, WALL);

    state->last_update = SDL_GetTicks();
    *appstate = state;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_AppState *state = (SDL_AppState *) appstate;

    SDL_Time now = SDL_GetTicks();
    if (now - state->last_update < 1000 / 30) { // ~30 FPS
        return SDL_APP_CONTINUE;
    }

    set_pixel(&state->game, 25, 25, SAND);

    update_game(&state->game);
    render_game(&state->game, state->game_tex, state->renderer);
    state->last_update = now;

    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255);
    SDL_RenderClear(state->renderer);
    SDL_FRect dstrect = {50, 50, 500, 500};
    SDL_RenderTexture(state->renderer, state->game_tex, NULL, &dstrect);
    SDL_RenderPresent(state->renderer);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_MOTION) {
        if (event->button.button == SDL_BUTTON_LEFT ||
            (event->type == SDL_EVENT_MOUSE_MOTION && (event->motion.state & SDL_BUTTON_LMASK))) {
            SDL_AppState *state = (SDL_AppState *) appstate;
            int mx = event->button.x;
            int my = event->button.y;
            int gx = (mx - 50) * state->game.width / 500;
            int gy = (my - 50) * state->game.height / 500;
            set_pixel(&state->game, gx, gy, SAND);
        } else if (event->button.button == SDL_BUTTON_RIGHT ||
                   (event->type == SDL_EVENT_MOUSE_MOTION &&
                    (event->motion.state & SDL_BUTTON_RMASK))) {
            SDL_AppState *state = (SDL_AppState *) appstate;
            int mx = event->button.x;
            int my = event->button.y;
            int gx = (mx - 50) * state->game.width / 500;
            int gy = (my - 50) * state->game.height / 500;
            set_pixel(&state->game, gx, gy, WATER);
        }
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_AppState *state = (SDL_AppState *) appstate;
    if (state) {
        if (state->renderer) {
            SDL_DestroyRenderer(state->renderer);
        }
        if (state->window) {
            SDL_DestroyWindow(state->window);
        }
        SDL_free(state);
    }
    SDL_Quit();
}
