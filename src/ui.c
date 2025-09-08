#include "ui.h"

Clay_RenderCommandArray create_game_scene_layout(SDL_Texture *game_tex,
                                                 Clay_ElementData *game_element)
{
    Clay_BeginLayout();

    CLAY({.id = CLAY_ID("root"),
          .layout =
              {
                  .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0)},
                  .padding = CLAY_PADDING_ALL(10),
                  .layoutDirection = CLAY_TOP_TO_BOTTOM,
              },
          .backgroundColor = {10, 10, 15, 255}})
    {
        CLAY({.id = CLAY_ID("title_text"),
              .layout =
                  {
                      .sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(50)},
                  },
              .backgroundColor = {0, 0, 255, 255}},
             CLAY_TEXT(CLAY_STRING("Falling Sand Simulator"),
                       CLAY_TEXT_CONFIG(
                           {.fontId = 1, .fontSize = 24, .textColor = {200, 200, 200, 255}})));
        CLAY({.id = CLAY_ID("container1"),
              .layout =
                  {
                      .sizing = {CLAY_SIZING_GROW(100), CLAY_SIZING_GROW(50)},
                      .padding = CLAY_PADDING_ALL(20),
                      .layoutDirection = CLAY_LEFT_TO_RIGHT,
                      .childGap = 20,
                  },
              .backgroundColor = {20, 20, 30, 255}})
        {
            CLAY({.id = CLAY_ID("game_texture"),
                  .layout =
                      {
                          .sizing = {CLAY_SIZING_GROW(100), CLAY_SIZING_GROW(100)},
                      },
                  .backgroundColor = {0, 0, 0, 255},
                  .image = {.imageData = game_tex},
                  .aspectRatio = 1.0f});
            CLAY({.id = CLAY_ID("MaterialPicker"),
                  .layout =
                      {
                          .sizing = {CLAY_SIZING_GROW(150), CLAY_SIZING_GROW(100)},
                          .padding = CLAY_PADDING_ALL(10),
                          .layoutDirection = CLAY_TOP_TO_BOTTOM,
                          .childGap = 10,
                      },
                  .backgroundColor = {30, 30, 40, 255}})
            {
                CLAY({.id = CLAY_ID("brush_sand"),
                      .layout =
                          {
                              .sizing = {CLAY_SIZING_GROW(100), CLAY_SIZING_FIT(40)},
                          },
                      .backgroundColor = {194, 178, 128, 255}},
                     CLAY_TEXT(CLAY_STRING("Sand"),
                               CLAY_TEXT_CONFIG(
                                   {.fontId = 0, .fontSize = 16, .textColor = {0, 0, 0, 255}})));
                CLAY({.id = CLAY_ID("brush_water"),
                      .layout =
                          {
                              .sizing = {CLAY_SIZING_GROW(100), CLAY_SIZING_FIT(40)},
                          },
                      .backgroundColor = {64, 164, 223, 255}},
                     CLAY_TEXT(CLAY_STRING("Water"),
                               CLAY_TEXT_CONFIG(
                                   {.fontId = 0, .fontSize = 16, .textColor = {0, 0, 0, 255}})));
                CLAY({.id = CLAY_ID("brush_wall"),
                      .layout =
                          {
                              .sizing = {CLAY_SIZING_GROW(100), CLAY_SIZING_FIT(40)},
                          },
                      .backgroundColor = {128, 128, 128, 255}},
                     CLAY_TEXT(CLAY_STRING("Wall"),
                               CLAY_TEXT_CONFIG(
                                   {.fontId = 0, .fontSize = 16, .textColor = {0, 0, 0, 255}})));
            }
        }
    }
    Clay_RenderCommandArray rcommands = Clay_EndLayout();
    if (game_element) {
        *game_element = Clay_GetElementData(CLAY_ID("game_texture"));
    }
    return rcommands;
}
