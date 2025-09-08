#include "game.h"
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>

const MaterialProperties material_properties[] = {
    {"EMPTY", SOLID, {0, 0, 0, 0}, 0},
    {"SAND", POWDER, {194, 178, 128, 255}, 10},
    {"WATER", LIQUID, {64, 164, 223, 255}, 5},
    {"WALL", SOLID, {128, 128, 128, 255}, 1000},
};

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

Material get_back_pixel(GameState *game, int x, int y)
{
    if (x >= 0 && x < game->width && y >= 0 && y < game->height) {
        return game->back[y * game->width + x];
    }
    Material empty = {0};
    return empty;
}

bool attempt_move(GameState *game, int x1, int y1, int x2, int y2)
{
    Material from = get_pixel(game, x1, y1);
    Material to = get_pixel(game, x2, y2);
    Material to_back = get_back_pixel(game, x2, y2);
    int from_mass = material_properties[from].mass;
    int to_mass = material_properties[to].mass;
    int to_back_mass = material_properties[to_back].mass;

    if (from_mass > to_mass && from_mass > to_back_mass) {
        set_back_pixel(game, x1, y1, to);
        set_pixel(game, x1, y1, EMPTY);
        set_back_pixel(game, x2, y2, from);
        set_pixel(game, x2, y2, EMPTY);
        return true;
    }
    return false;
}

void move_material(GameState *game, int x, int y)
{
    Material mat = get_pixel(game, x, y);

    if (material_properties[mat].type == SOLID) {
        set_back_pixel(game, x, y, mat);
        return;
    }

    if (attempt_move(game, x, y, x, y + 1)) {
        return;
    }

    int x_dir = (rand() % 2) ? 1 : -1;
    int y_dir = material_properties[mat].type == LIQUID ? 0 : 1;
    if (attempt_move(game, x, y, x + x_dir, y + y_dir)) {
        return;
    }

    if (attempt_move(game, x, y, x - x_dir, y + y_dir)) {
        return;
    }
    set_back_pixel(game, x, y, mat);
}

void update_game(GameState *game)
{
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            Material mat = get_pixel(game, x, y);
            if (mat != EMPTY) {
                move_material(game, x, y);
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

Material get_pixel(const GameState *game, int x, int y)
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