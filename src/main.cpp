#include <SDL3/SDL.h>
#include <GL/gl.h>

#include "renderer.h"
#include "block.h"
#include "selection.h"

int main() {
    SDL_Window* window = SDL_CreateWindow("", 768, 512, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    bool running = true;

    World world;
    memset(world, 0, sizeof(World));
    world[15][0][15] = Block_Dirt;

    while (running) {
        bool mouse_left  = false;
        bool mouse_right = false;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT)  mouse_left  = true;
                if (event.button.button == SDL_BUTTON_RIGHT) mouse_right = true;
            }
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        Selection* selection = get_selection(world, window);

        prepare_rendering();
        draw_grid();
        draw_voxels(world);
        draw_selection(selection);

        if (mouse_left) world[selection->pos.x][selection->pos.y][selection->pos.z] = 0;
        if (mouse_right) {
            IVec3 pos = selection->pos + selection->normal;
            if (pos.x >= 0 && pos.y >= 0 && pos.z >= 0 && pos.x < WORLD_SIZE && pos.y < WORLD_SIZE && pos.z < WORLD_SIZE) world[pos.x][pos.y][pos.z] = Block_Dirt;
        }

        free(selection);

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    return 0;
}
