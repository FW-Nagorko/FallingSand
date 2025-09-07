#include "rendering.h"
#include "game.h"

void render_game(GameState *game, SDL_Texture *target, SDL_Renderer *renderer) {
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            Material mat = get_pixel(game, x, y);
            switch (mat) {
            case SAND:
                SDL_SetRenderDrawColor(renderer, 194, 178, 128, 255);
                break;
            case WATER:
                SDL_SetRenderDrawColor(renderer, 64, 164, 223, 255);
                break;
            case WALL:
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                break;
            case EMPTY:
            default:
                continue;
            }
            SDL_FRect rect = {x, y, 1, 1};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
}
