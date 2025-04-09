#include "types.h"

#include "renderer.h"

#include <GL/gl.h>

#include <string>
#include <fstream>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image.h"
#include "lib/stb_image_write.h"
#include "lib/portable-file-dialogs.h"

std::istream& operator>>(std::istream& stream, BlockID& value) {
    unsigned char raw;
    if (stream >> raw) value = (BlockID)raw;
    return stream;
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
    std::ifstream stream = std::ifstream(filename);
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_SIZE; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                stream >> world[x][y][z];
            }
        }
    }
    stream.close();
}

void write_project(World world) {
    std::string filename = save_file("Save Project", "BTCB World Map Project", "*.wrl");
    if (filename.empty()) return;
    std::ofstream stream = std::ofstream(filename);
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_SIZE; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                stream << world[x][y][z];
            }
        }
    }
    stream.close();
}

void export_project(World world) {
    std::string filename = save_file("Export Project", "PNG Image", "*.png");
    if (filename.empty()) return;

    prepare_rendering();

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_voxels(world);

    struct Pixel { uint8_t r, g, b, a; };
    Pixel* pixels    = (Pixel*)malloc(sizeof(Pixel) * 768 * 512);
    Pixel* processed = (Pixel*)malloc(sizeof(Pixel) * 384 * 256);
    glReadPixels(0, 0, 768, 512, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 384; x++) {
            processed[y * 384 + x] = pixels[(511 - y * 2) * 768 + x * 2];
        }
    }

    stbi_write_png(filename.c_str(), 384, 256, 4, (int*)processed, 0);
}

void read_tileset(GLuint* texture) {
    std::string filename = open_file("Load Image", "PNG Image", "*.png");
    if (filename.empty()) return;

    int x, y, c;
    unsigned char* image = stbi_load(filename.c_str(), &x, &y, &c, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TILEMAP_WIDTH, TILEMAP_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
}