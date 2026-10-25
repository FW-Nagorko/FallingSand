#include "ui.h"
#include "game.h"
#include <stdint.h>
#include "../vendor/Vector/Vector.h"

Clay_String to_clay_string(const char *str)
{
    Clay_String clay_str;
    clay_str.chars = str;
    clay_str.length = (int32_t) strlen(str);
    clay_str.isStaticallyAllocated = true;
    return clay_str;
}

typedef struct handler_data {
    Material *brush;
    Material button_mat;
} Handler;

void pick_element_button_handler(Clay_ElementId elementId, Clay_PointerData pointerInfo,
                                 intptr_t userData)
{
    Handler *handler_data = (Handler *) userData;
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        *handler_data->brush = handler_data->button_mat;
    }
}

Vector(Handler) handler_contexts = NULL;

void create_handlers(Material *brush)
{
    if (vector_size(handler_contexts) != vector_size(material_properties))
        for_vector(material_properties, idx)
        {
                vector_push_back(handler_contexts, (Handler){.brush = brush, .button_mat = idx});
        }
}

void pick_element_button(Material mat, Handler *handler)
{
    Clay_String label;

    char id_buffer[32];
    memset(id_buffer, 0, sizeof(id_buffer));
    strcat(id_buffer, "brush_");
    strcat(id_buffer, material_properties[mat].name);

    label = to_clay_string(material_properties[mat].name);

    CLAY({.id = CLAY_SID(to_clay_string(id_buffer)),
          .layout =
              {
                  .sizing = {CLAY_SIZING_GROW(100), CLAY_SIZING_FIT(40)},
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
              },
          .backgroundColor = {material_properties[mat].color[0], material_properties[mat].color[1],
                              material_properties[mat].color[2], 255}})

    {
        Clay_OnHover(pick_element_button_handler, (intptr_t) handler);
        CLAY_TEXT(label, CLAY_TEXT_CONFIG({.fontId = 0, .textColor = {255, 255, 255, 255}}));
    }
}

void element_picker(Material *brush_state)
{
    create_handlers(brush_state);
    for_vector(handler_contexts, idx)
    {
        pick_element_button(handler_contexts[idx].button_mat, &handler_contexts[idx]);
    }
}
Clay_RenderCommandArray create_game_scene_layout(SDL_Texture *game_tex,
                                                 Clay_ElementData *game_element,
                                                 Material *brush_state)
{
    create_handlers(brush_state);
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
        CLAY({
            .id = CLAY_ID("title_text"),
            .layout = {.sizing = {CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(50)},
                       .padding = 10,
                       .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER}},
        })
        {
            CLAY_TEXT(
                CLAY_STRING("Falling Sand Simulator"),
                CLAY_TEXT_CONFIG({.fontId = 1, .fontSize = 32, .textColor = {200, 200, 200, 255}}));
        }
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
                  .aspectRatio = {1.0f}});
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
                element_picker(brush_state);
            }
        }
    }
    Clay_RenderCommandArray rcommands = Clay_EndLayout();
    if (game_element) {
        *game_element = Clay_GetElementData(CLAY_ID("game_texture"));
    }
    return rcommands;
}
