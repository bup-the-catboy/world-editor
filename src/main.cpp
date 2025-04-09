#include <SDL3/SDL.h>
#include <GL/gl.h>

#include "renderer.h"
#include "selection.h"
#include "io.h"

int main() {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow("", 768, 512, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    bool running = true;

    World world;
    BlockID curr_block = Block_Water;
    BlockID selected_block = Block_Air;
    memset(world, 0, sizeof(World));

    bool selection_active = false;
    bool ctrl = false;
    float sel_x, sel_y;

    while (running) {
        bool mouse_left  = false;
        bool mouse_right = false;

        float mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT)  mouse_left  = true;
                if (event.button.button == SDL_BUTTON_RIGHT) mouse_right = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_LSHIFT) {
                    sel_x = mouse_x;
                    sel_y = mouse_y;
                    selection_active = true;
                }
                if (event.key.key == SDLK_LCTRL) ctrl = true;
                if (ctrl) {
                    if (event.key.key == SDLK_S) write_project(world);
                    if (event.key.key == SDLK_E) export_project(world);
                    if (event.key.key == SDLK_O) read_project(world);
                    if (event.key.key == SDLK_L) read_tileset(&tileset_texture);
                    if (event.key.key == SDLK_N) memset(world, 0, sizeof(World));
                }
            }
            if (event.type == SDL_EVENT_KEY_UP) {
                if (event.key.key == SDLK_LSHIFT) {
                    curr_block = selected_block;
                    selection_active = false;
                }
                if (event.key.key == SDLK_LCTRL) ctrl = false;
            }
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        prepare_rendering();

        Selection* selection = get_selection(world, window);
        if (selection_active) selection->pos = IVec3(-1, -1, -1);

        draw_grid();
        draw_voxels(world);
        draw_selection(selection);
        if (selection_active) selected_block = draw_block_selection(sel_x, sel_y, mouse_x - sel_x, mouse_y - sel_y, curr_block);

        if (mouse_left) world[selection->pos.x][selection->pos.y][selection->pos.z] = Block_Air;
        if (mouse_right) {
            IVec3 pos = selection->pos + selection->normal;
            if (pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < WORLD_SIZE && pos.y < WORLD_SIZE && pos.z < WORLD_SIZE) world[pos.x][pos.y][pos.z] = curr_block;
        }

        free(selection);

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    return 0;
}
