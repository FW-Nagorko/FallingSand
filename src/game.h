#ifndef GAME_H
#define GAME_H
#include <stdlib.h>

typedef enum { EMPTY, SAND, WATER, WALL } Material;

typedef enum { SOLID, LIQUID, POWDER } MaterialType;

typedef struct {
    char name[16];
    MaterialType type;
    unsigned char color[4];
    int mass;
} MaterialProperties;

typedef struct {
    int width, height;
    Material *front;
    Material *back;
} GameState;

// TODO: add loading materials from a file
extern const MaterialProperties material_properties[];

void init_game(GameState *game, int width, int height);
void update_game(GameState *game);
void set_pixel(GameState *game, int x, int y, Material mat);
Material get_pixel(const GameState *game, int x, int y);
void free_game(GameState *game);

#endif