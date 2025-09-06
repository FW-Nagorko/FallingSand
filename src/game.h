#ifndef GAME_H
#define GAME_H


typedef enum {
    EMPTY,
    SAND,
    WATER,
    WALL
} Material;

typedef struct {
    int width, height;
    Material *front;
    Material *back;
} GameState;

void init_game(GameState *game, int width, int height);
void update_game(GameState *game);
void set_pixel(GameState *game, int x, int y, Material mat);
Material get_pixel(GameState *game, int x, int y);
void free_game(GameState *game);

#endif