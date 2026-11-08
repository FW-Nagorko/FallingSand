#include "game.h"
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

Vector(MaterialProperties) material_properties = NULL;
Vector(MaterialReaction) material_reactions = NULL;

Material get_material_by_name(const char* name) {
    for_vector(material_properties, mat){
        if(strcmp(material_properties[mat].name, name) == 0) return mat;
    }
    return MAT_EMPTY;
}

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
                     (MaterialProperties) {"FIRE", LIQUID, {253, 94, 94, 255}, -6});
    vector_push_back(material_properties,
                     (MaterialProperties) {"WALL", SOLID, {128, 128, 128, 255}, 1000});

    vector_push_back(material_reactions,
                    (MaterialReaction) {{get_material_by_name("STEAM"), MAT_ANY}, {get_material_by_name("WATER"), MAT_ANY}, 0.01});
    vector_push_back(material_reactions,
                    (MaterialReaction) {{get_material_by_name("FIRE"), MAT_ANY}, {get_material_by_name("EMPTY"), MAT_ANY}, 0.05});
    vector_push_back(material_reactions,
                    (MaterialReaction) {{get_material_by_name("WATER"), get_material_by_name("SAND"), MAT_ANY, MAT_ANY}, {get_material_by_name("DIRT"), MAT_ANY, MAT_ANY, MAT_ANY}, 0.1});
    vector_push_back(material_reactions,
                    (MaterialReaction) {{get_material_by_name("OIL"), get_material_by_name("FIRE"), MAT_ANY, MAT_ANY}, {get_material_by_name("FIRE"), get_material_by_name("FIRE"), MAT_ANY, MAT_ANY}, 0.5});
    vector_push_back(material_reactions,
                    (MaterialReaction) {{get_material_by_name("WATER"), get_material_by_name("FIRE"), MAT_ANY, MAT_ANY}, {get_material_by_name("STEAM"), MAT_ANY, MAT_ANY, MAT_ANY}, 0.8});
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


static inline bool match_reaction(Material *const c[4], const MaterialReaction *r, int slot[4])
{
    unsigned char taken = 0;
    for (int i = 0; i < 4; i++) {
        int found = -1;
        for (int j = 0; j < 4; j++) {
            if (taken >> j & 1) continue;
            if (r->inputs[j] == *c[i] || r->inputs[j] == MAT_ANY) { found = j; break; }
        }
        if (found < 0) return false;
        taken |= 1u << found;
        slot[i] = found;
    }
    return true;
}

static inline bool react_particles(Material *tl, Material *tr, Material *bl, Material *br)
{
    Material *c[4] = { tl, tr, bl, br };
    int slot[4];

    for_vector(material_reactions, idx) {
        const MaterialReaction *r = &material_reactions[idx];

        if (!match_reaction(c, r, slot)) continue;
        if ((float)rand() / (float)RAND_MAX >= r->probability) continue;

        for (int i = 0; i < 4; i++) {
            Material out = r->outputs[slot[i]];
            if (out != MAT_ANY) *c[i] = out;
        }
        return true;
    }
    return false;
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
            if(!react_particles(tl, tr, bl, br))
                move_particles(tl, tr, bl, br);
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
