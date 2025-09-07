#include "game.h"
#include <memory.h>
#include <stdlib.h>

void init_game(GameState *game, int width, int height)
{
    game->width = width;
    game->height = height;
    game->front = (Material *) malloc(width * height * sizeof(Material));
    game->back = (Material *) malloc(width * height * sizeof(Material));
    memset(game->front, 0, width * height * sizeof(Material));
    memset(game->back, 0, width * height * sizeof(Material));
}

void swap_buffers(GameState *game);
void set_back_pixel(GameState *game, int x, int y, Material mat)
{
    if (x >= 0 && x < game->width && y >= 0 && y < game->height) {
        game->back[y * game->width + x] = mat;
    }
}

void update_game(GameState *game)
{
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            Material mat = get_pixel(game, x, y);
            switch (mat) {
            case SAND:
                if (get_pixel(game, x, y + 1) == EMPTY) {
                    set_back_pixel(game, x, y + 1, SAND);
                } else if (get_pixel(game, x - 1, y + 1) == EMPTY) {
                    set_back_pixel(game, x - 1, y + 1, SAND);
                } else if (get_pixel(game, x + 1, y + 1) == EMPTY) {
                    set_back_pixel(game, x + 1, y + 1, SAND);
                } else {
                    set_back_pixel(game, x, y, SAND);
                }
                break;
            case WATER:
                if (get_pixel(game, x, y + 1) == EMPTY) {
                    set_back_pixel(game, x, y + 1, WATER);
                } else if (get_pixel(game, x - 1, y + 1) == EMPTY) {
                    set_back_pixel(game, x - 1, y + 1, WATER);
                } else if (get_pixel(game, x + 1, y + 1) == EMPTY) {
                    set_back_pixel(game, x + 1, y + 1, WATER);
                } else {
                    set_back_pixel(game, x, y, WATER);
                }
                break;
            case EMPTY:
                break;
            default:
                set_back_pixel(game, x, y, mat);
                break;
            }
        }
    }
    swap_buffers(game);
    memset(game->back, 0, game->width * game->height * sizeof(Material));
}

void set_pixel(GameState *game, int x, int y, Material mat)
{
    if (x >= 0 && x < game->width && y >= 0 && y < game->height) {
        game->front[y * game->width + x] = mat;
    }
}

void swap_buffers(GameState *game)
{
    Material *temp = game->front;
    game->front = game->back;
    game->back = temp;
}

Material get_pixel(GameState *game, int x, int y)
{
    if (x >= 0 && x < game->width && y >= 0 && y < game->height) {
        return game->front[y * game->width + x];
    }
    Material empty = {0};
    return empty;
}

void free_game(GameState *game)
{
    free(game->front);
}