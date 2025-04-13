#include "types.h"

#include "renderer.h"

#include <GL/gl.h>

#include <string>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"
#include "lib/portable-file-dialogs.h"

struct Pixel {
    uint8_t r, g, b, a;
};

struct Image {
    int width, height;
    Pixel* pixels;
    Pixel& px(int x, int y) {
        return pixels[y * width + x];
    }
};

Image* create_image(int width, int height) {
    Image* image  = (Image*)malloc(sizeof(Image));
    image->pixels = (Pixel*)malloc(sizeof(Pixel) * width * height);
    image->width  = width;
    image->height = height;
    return image;
}

void blit_image(Image* out, Image* in, int x, int y, int w, int h) {
    for (int Y = 0; Y < h; Y++) {
        for (int X = 0; X < w; X++) {
            out->px(X + x, Y + y) = in->px(X * in->width / w, in->height - 1 - (Y * in->height / h));
        }
    }
}

void free_image(Image* image) {
    free(image->pixels);
    free(image);
}

std::string open_file(std::string title, std::string filter_name, std::string filter) {
    std::vector<std::string> files = pfd::open_file(title, ".", { filter_name, filter }).result();
    if (files.empty()) return "";
    return files[0];
}

std::string save_file(std::string title, std::string filter_name, std::string filter_ext) {
    return pfd::save_file(title, ".", { filter_name, filter_ext }).result();
}

void read_project(World world) {
    std::string filename = open_file("Open Project", "BTCB World Map Project", "*.wrl");
    if (filename.empty()) return;
    FILE* f = fopen(filename.c_str(), "r");
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_SIZE; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                fread(&world[x][y][z], 1, 1, f);
            }
        }
    }
    fclose(f);
}

void write_project(World world) {
    std::string filename = save_file("Save Project", "BTCB World Map Project", "*.wrl");
    if (filename.empty()) return;
    FILE* f = fopen(filename.c_str(), "w");
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_SIZE; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                fwrite(&world[x][y][z], 1, 1, f);
            }
        }
    }
    fclose(f);
}

void render_world(World world, WorldContext context, int anim_frame, Image* image) {
    prepare_rendering();
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    draw_voxels(world, context, anim_frame);
    glReadPixels(0, 0, 768, 512, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
}

void export_project(World world) {
    std::string filename = save_file("Export Project", "PNG Image", "*.png");
    if (filename.empty()) return;

    Image* output  = create_image(384 * 4, 256 * 2); // 4 animation states * fg,bg

    for (int i = 0; i < 4; i++) {
        for (WorldContext ctx : { BackgroundOnly, ForegroundOnly }) {
            Image* texture = create_image(768, 512);
            render_world(world, ctx, i, texture);
            blit_image(output, texture, i * 384, ctx * 256, 384, 256);
            free_image(texture);
        }
    }

    stbi_write_png(filename.c_str(), output->width, output->height, 4, output->pixels, 0);
}

void read_tileset(GLuint* texture) {
    std::string filename = open_file("Load Image", "PNG Image", "*.png");
    if (filename.empty()) return;

    int x, y, c;
    unsigned char* image = stbi_load(filename.c_str(), &x, &y, &c, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TILEMAP_WIDTH, TILEMAP_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
}