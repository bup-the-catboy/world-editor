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
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        Selection* selection = get_selection(world, window);

        prepare_rendering();
        draw_grid();
        draw_voxels(world);
        draw_selection(selection);

        free(selection);

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    return 0;
}
