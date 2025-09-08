#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../vendor/clay/clay.h"

Clay_RenderCommandArray create_game_scene_layout(SDL_Texture *game_tex,
                                                 Clay_ElementData *game_element);

#endif // UI_H