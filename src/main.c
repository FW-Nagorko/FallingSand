// SDL includes
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unistd.h>

// Clay includes
#define CLAY_IMPLEMENTATION
#include "../vendor/clay/clay.h"
#include "../vendor/clay/SDL3_clay_renderer.c"

// Project includes
#include "game.h"
#include "rendering.h"
#include "ui.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    GameState game;
    SDL_Time last_update;
    SDL_Texture *game_tex;
    Clay_SDL3RendererData clay_renderer;
    SDL_Time last_scroll_update;
    Clay_Vector2 mouse_pos;
    bool mouse_down;
    Clay_ElementData game_element;
    Material brush;
} SDL_AppState;

Clay_Vector2 translate_screen_to_game(Clay_Vector2 screen_pos, Clay_ElementData *game_element,
                                      int width, int height)
{
    if (!game_element->found) {
        return (Clay_Vector2){-1, -1};
    }
    float relative_x = screen_pos.x - game_element->boundingBox.x;
    float relative_y = screen_pos.y - game_element->boundingBox.y;
    if (relative_x < 0 || relative_x >= game_element->boundingBox.width || relative_y < 0 ||
        relative_y >= game_element->boundingBox.height) {
        return (Clay_Vector2){-1, -1};
    }
    float game_x = (relative_x / game_element->boundingBox.width) * width;
    float game_y = (relative_y / game_element->boundingBox.height) * height;
    return (Clay_Vector2){game_x, game_y};
}

static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config,
                                              void *userData)
{
    TTF_Font **fonts = userData;
    TTF_Font *font = fonts[config->fontId];
    int width, height;

    TTF_SetFontSize(font, config->fontSize);
    if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s", SDL_GetError());
    }

    return (Clay_Dimensions){(float) width, (float) height};
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_AppState *state = (SDL_AppState *) SDL_calloc(1, sizeof(SDL_AppState));
    if (!state) {
        SDL_Log("Failed to allocate app state: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Falling Sand", 800, 600, 0, &state->window,
                                     &state->renderer)) {
        SDL_Log("Failed to create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    init_game(&state->game, 50, 50);

    state->game_tex =
        SDL_CreateTexture(state->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                          state->game.width, state->game.height);

    if (!state->game_tex) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetTextureScaleMode(state->game_tex, SDL_SCALEMODE_NEAREST);

    uint64_t arena_size = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(arena_size, SDL_malloc(arena_size));

    Clay_Initialize(arena, (Clay_Dimensions){800, 600},
                    (Clay_ErrorHandler){.errorHandlerFunction = NULL, .userData = NULL});

    if (!TTF_Init()) {
        SDL_Log("Failed to initialize TTF: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->clay_renderer.renderer = state->renderer;
    state->clay_renderer.textEngine = TTF_CreateRendererTextEngine(state->renderer);

    if (!state->clay_renderer.textEngine) {
        SDL_Log("Failed to create text engine: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    state->clay_renderer.fonts = SDL_calloc(2, sizeof(TTF_Font *));
    state->clay_renderer.fonts[0] = TTF_OpenFont("assets/VCR_OSD_MONO.ttf", 20.0f);
    state->clay_renderer.fonts[1] = TTF_OpenFont("assets/ka1.ttf", 16.0f);

    if (!state->clay_renderer.fonts[0] || !state->clay_renderer.fonts[1]) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    Clay_SetMeasureTextFunction(SDL_MeasureText, state->clay_renderer.fonts);

    state->brush = SAND;
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

    if (state->mouse_down) {
        Clay_Vector2 game_pos = translate_screen_to_game(state->mouse_pos, &state->game_element,
                                                         state->game.width, state->game.height);
        int gx = (int) game_pos.x;
        int gy = (int) game_pos.y;
        if (gx >= 0 && gx < state->game.width && gy >= 0 && gy < state->game.height) {
            set_pixel(&state->game, gx, gy, state->brush);
        }
    }

    update_game(&state->game);
    render_game(&state->game, state->game_tex, state->renderer);
    state->last_update = now;

    Clay_RenderCommandArray rcommands =
        create_game_scene_layout(state->game_tex, &state->game_element, &state->brush);

    SDL_Clay_RenderClayCommands(&state->clay_renderer, &rcommands);
    SDL_RenderPresent(state->renderer);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    SDL_AppState *state = (SDL_AppState *) appstate;
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        Clay_SetLayoutDimensions(
            (Clay_Dimensions){.width = event->window.data1, .height = event->window.data2});
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP ||
        event->type == SDL_EVENT_MOUSE_MOTION) {
        state->mouse_pos = (Clay_Vector2){.x = event->motion.x, .y = event->motion.y};
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)
            state->mouse_down = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                                event->button.button == SDL_BUTTON_LEFT;
        Clay_SetPointerState(state->mouse_pos, state->mouse_down);
    }

    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        float deltaTime = (SDL_GetTicks() - state->last_scroll_update) / 1000.0f;
        Clay_UpdateScrollContainers(true, (Clay_Vector2){.x = event->wheel.x, .y = event->wheel.y},
                                    deltaTime);
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
        if (state->game_tex) {
            SDL_DestroyTexture(state->game_tex);
        }
        free_game(&state->game);
        if (state->clay_renderer.fonts) {
            TTF_CloseFont(state->clay_renderer.fonts[0]);
            TTF_CloseFont(state->clay_renderer.fonts[1]);
        }
        if (state->clay_renderer.textEngine) {
            TTF_DestroyRendererTextEngine(state->clay_renderer.textEngine);
        }
    }
    SDL_Quit();
}
