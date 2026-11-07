#include "game.h"
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

Vector(MaterialProperties) material_properties = NULL;

void init_game(GameState *game, int width, int height)
{
    game->width = width;
    game->height = height;
    game->reverse_x = 0;
    game->world = (Material *) malloc(width * height * sizeof(Material));
    game->tick = 0;
    memset(game->world, 0, width * height * sizeof(Material));
    vector_push_back(material_properties, (MaterialProperties) {"EMPTY", LIQUID, {0, 0, 0, 0}, 0});
    vector_push_back(material_properties,
                     (MaterialProperties) {"SAND", POWDER, {194, 178, 128, 255}, 10});
    vector_push_back(material_properties,
                     (MaterialProperties) {"DIRT", POWDER, {165, 42, 42, 255}, 15});
    vector_push_back(material_properties,
                     (MaterialProperties) {"WATER", LIQUID, {64, 164, 223, 255}, 5});
    vector_push_back(material_properties,
                     (MaterialProperties) {"OIL", LIQUID, {80, 80, 80, 255}, 7});
    vector_push_back(material_properties,
                     (MaterialProperties) {"STEAM", LIQUID, {94, 194, 253, 255}, -5});
    vector_push_back(material_properties,
                     (MaterialProperties) {"WALL", SOLID, {128, 128, 128, 255}, 1000});
}


static inline bool displaces(Material a, Material b)
{
    return material_properties[b].type != SOLID &&
           material_properties[a].type != SOLID &&
           material_properties[a].mass > material_properties[b].mass;
}

static inline void swp(Material *restrict a, Material *restrict b)
{
    Material t = *a;
    *a = *b;
    *b = t;
}

static inline bool is_powder(Material m) { return material_properties[m].type == POWDER; }
static inline bool is_solid(Material m) { return material_properties[m].type == SOLID; }
static inline bool is_liquid(Material m) { return material_properties[m].type == LIQUID; }

static inline void move_particles(Material *tl, Material *tr, Material *bl, Material *br)
{
    
            // 1. pionowo
            if (displaces(*tl, *bl))
                swp(tl, bl);
            if (displaces(*tr, *br))
                swp(tr, br);

            // 2. po przekątnej (proszki, gdy ruch w dół zablokowany)
            if (rand() & 1) {
                if (is_powder(*tl) && !displaces(*tl, *bl) && displaces(*tl, *br))
                    swp(tl, br);
                else if (is_powder(*tr) && !displaces(*tr, *br) && displaces(*tr, *bl))
                    swp(tr, bl);
            } else {
                if (is_powder(*tr) && !displaces(*tr, *br) && displaces(*tr, *bl))
                    swp(tr, bl);
                else if (is_powder(*tl) && !displaces(*tl, *bl) && displaces(*tl, *br))
                    swp(tl, br);
            }

            // 3. poziomo (ciecze)
            if ((is_liquid(*bl) || is_liquid(*br)) && (rand() % 3) <= 1) {
                if (is_liquid(*bl) && displaces(*bl, *br))
                    swp(bl, br);
                else if (is_liquid(*br) && displaces(*br, *bl))
                    swp(bl, br);
            }
            if ((is_liquid(*tl) || is_liquid(*tr)) && (rand() % 3) <= 1) {
                if (is_liquid(*tl) && displaces(*tl, *tr))
                    swp(tl, tr);
                else if (is_liquid(*tr) && displaces(*tr, *tl))
                    swp(tl, tr);
            }
}

static inline void react_particles(Material *tl, Material *tr, Material *bl, Material *br)
{
    
}

void update_game(GameState *g)
{
    int w = g->width;
    int h = g->height;
    int off = g->tick++ & 1;

    for (int y = off; y + 1 < h; y += 2) {
        for (int x = off; x + 1 < w; x += 2) {
            Material *tl = &g->world[y * w + x], *tr = tl + 1;
            Material *bl = tl + w, *br = bl + 1;
            move_particles(tl, tr, bl, br);
            react_particles(tl, tr, bl, br);
        }
    }
}

void set_pixel(GameState *game, int x, int y, Material mat)
{
    if (x >= 0 && x < game->width && y >= 0 && y < game->height) {
        game->world[y * game->width + x] = mat;
    }
}

Material get_pixel(const GameState *game, int x, int y)
{
    if (x >= 0 && x < game->width && y >= 0 && y < game->height) {
        return game->world[y * game->width + x];
    }
    return EMPTY;
}

void free_game(GameState *game)
{
    free(game->world);
    vector_free(material_properties);
}
