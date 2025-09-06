#ifndef RENDERING_H
#define RENDERING_H

#include "game.h"
#include <SDL3/SDL.h>

void render_game(GameState *game, SDL_Texture *target, SDL_Renderer *renderer);

#endif