#ifndef GAME_H
#define GAME_H
#include "../vendor/Vector/Vector.h"

#define SAND  1
typedef char Material;

typedef enum { EMPTY, SOLID, LIQUID, POWDER } MaterialType;

typedef struct {
    char name[16];
    MaterialType type;
    unsigned char color[4];
    int mass;
} MaterialProperties;

typedef struct {
    int input_a;
    int input_b;
    int output_a;
    int output_b;
    float probability;
} MaterialReactions;

typedef struct {
    int width, height;
    Material *world;
    int reverse_x;
    size_t tick;
} GameState;

// TODO: add loading materials from a file
extern Vector(MaterialProperties) material_properties;
extern Vector(MaterialReactions) material_reactions;

void init_game(GameState *game, int width, int height);
void update_game(GameState *game);
void set_pixel(GameState *game, int x, int y, Material mat);
Material get_pixel(const GameState *game, int x, int y);
void free_game(GameState *game);

#endif
