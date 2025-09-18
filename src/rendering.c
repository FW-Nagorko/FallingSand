#include "rendering.h"
#include "game.h"

void render_game(GameState *game, SDL_Texture *target, SDL_Renderer *renderer)
{
    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            Material mat = get_pixel(game, x, y);
            SDL_SetRenderDrawColor(renderer, material_properties[mat].color[0],
                                   material_properties[mat].color[1],
                                   material_properties[mat].color[2], 255);
            SDL_FRect rect = {x, y, 1, 1};
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
}
