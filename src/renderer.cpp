#include "types.h"

#include <GL/gl.h>
#include <stdio.h>

#include <vector>

#include "block.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SCALE 6

static int num_frames = 0;

struct Vertex {
    Vec3 xyz;
    Vec2 uv;
};

//Mtx mtx_projection = Mtx::perspective(70, 3/2.f, .1f, 100.f);
Mtx mtx_projection = Mtx::orthographic(-SCALE, SCALE, -SCALE * 2/3.f, SCALE * 2/3.f, .1f, 100.f);
Mtx mtx_modelview  = Mtx::identity()
    * Mtx::pitch(19.5 * Angle::rad)
    * Mtx::yaw(135 * Angle::rad)
    * Mtx::translate(0, -8, 0)
;
std::vector<Mtx> matrices = {};

void push_matrix(Mtx mtx) {
    matrices.push_back(matrices.back() * mtx);
}

void pop_matrix() {
    matrices.pop_back();
}

void put_vertex(float x, float y, float z, float u = 0, float v = 0) {
    Mtx m = matrices.back();
    Vec3 vec = (mtx_projection * matrices.back() * Vec4(x, y, z, 1)).divide().vec3();
    glTexCoord2f(u, v);
    glVertex3f(vec.x, vec.y, vec.z);
}

void render_begin() {
    glBegin(GL_QUADS);
}

void render_end() {
    glEnd();
    glFlush();
}

void prepare_rendering() {
    if (num_frames == 0) {
        int x, y, c;
        unsigned char* image = stbi_load("../assets/images/tilesets/grass_map.png", &x, &y, &c, 4);
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TILEMAP_WIDTH, TILEMAP_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    matrices.clear();
    matrices.push_back(mtx_modelview);

    num_frames++;
}

void draw_grid() {
    glColor4f(.5f, .5f, .5f, 1.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    render_begin();
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int z = 0; z < WORLD_SIZE; z++) {
            put_vertex(x + 0, 0, z + 0);
            put_vertex(x + 1, 0, z + 0);
            put_vertex(x + 1, 0, z + 1);
            put_vertex(x + 0, 0, z + 1);
        }
    }
    render_end();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

#define SIDES(x) x, x, x, x
#define FACES(x) x, x, x, x, x, x

void draw_xplane(Vec2 from, Vec2 to, float x, IVec4 uv = IVec4()) {
    put_vertex(x, from.x, from.y, (uv.x       ) / (float)TILEMAP_WIDTH, (uv.y + uv.w) / (float)TILEMAP_HEIGHT);
    put_vertex(x,  to .x, from.y, (uv.x       ) / (float)TILEMAP_WIDTH, (uv.y       ) / (float)TILEMAP_HEIGHT);
    put_vertex(x,  to .x,  to .y, (uv.x + uv.z) / (float)TILEMAP_WIDTH, (uv.y       ) / (float)TILEMAP_HEIGHT);
    put_vertex(x, from.x,  to .y, (uv.x + uv.z) / (float)TILEMAP_WIDTH, (uv.y + uv.w) / (float)TILEMAP_HEIGHT);
}

void draw_yplane(Vec2 from, Vec2 to, float y, IVec4 uv = IVec4()) {
    put_vertex(from.x, y, from.y, (uv.x + uv.z) / (float)TILEMAP_WIDTH, (uv.y + uv.w) / (float)TILEMAP_HEIGHT);
    put_vertex( to .x, y, from.y, (uv.x       ) / (float)TILEMAP_WIDTH, (uv.y + uv.w) / (float)TILEMAP_HEIGHT);
    put_vertex( to .x, y,  to .y, (uv.x       ) / (float)TILEMAP_WIDTH, (uv.y       ) / (float)TILEMAP_HEIGHT);
    put_vertex(from.x, y,  to .y, (uv.x + uv.z) / (float)TILEMAP_WIDTH, (uv.y       ) / (float)TILEMAP_HEIGHT);
}

void draw_zplane(Vec2 from, Vec2 to, float z, IVec4 uv = IVec4()) {
    put_vertex(from.x, from.y, z, (uv.x + uv.z) / (float)TILEMAP_WIDTH, (uv.y + uv.w) / (float)TILEMAP_HEIGHT);
    put_vertex( to .x, from.y, z, (uv.x       ) / (float)TILEMAP_WIDTH, (uv.y + uv.w) / (float)TILEMAP_HEIGHT);
    put_vertex( to .x,  to .y, z, (uv.x       ) / (float)TILEMAP_WIDTH, (uv.y       ) / (float)TILEMAP_HEIGHT);
    put_vertex(from.x,  to .y, z, (uv.x + uv.z) / (float)TILEMAP_WIDTH, (uv.y       ) / (float)TILEMAP_HEIGHT);
}

void draw_box(Vec3 from, Vec3 to, IVec4 posy = IVec4(), IVec4 negy = IVec4(), IVec4 posx = IVec4(), IVec4 negx = IVec4(), IVec4 posz = IVec4(), IVec4 negz = IVec4()) {
    draw_xplane(Vec2(from.y, from.z), Vec2(to.y, to.z), from.x, negx);
    draw_xplane(Vec2(from.y, from.z), Vec2(to.y, to.z),  to .x, posx);
    draw_yplane(Vec2(from.x, from.z), Vec2(to.x, to.z), from.y, negy);
    draw_yplane(Vec2(from.x, from.z), Vec2(to.x, to.z),  to .y, posy);
    draw_zplane(Vec2(from.x, from.y), Vec2(to.x, to.y), from.z, negz);
    draw_zplane(Vec2(from.x, from.y), Vec2(to.x, to.y),  to .z, posz);
}

void draw_cube(IVec4 posy = IVec4(), IVec4 negy = IVec4(), IVec4 posx = IVec4(), IVec4 negx = IVec4(), IVec4 posz = IVec4(), IVec4 negz = IVec4()) {
    draw_box(Vec3(0, 0, 0), Vec3(1, 1, 1), posy, negy, posx, negx, posz, negz);
}

void draw_voxels(World world) {
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    render_begin();
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_SIZE; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                if (world[x][y][z] == Block_Air) continue;
                push_matrix(Mtx::translate(x, y, z));
                switch ((BlockID)world[x][y][z]) {
                    case Block_Air: break;
                    case Block_Dirt: draw_cube(FACES(IVec4(32, 0, 16, 16))); break;
                }
                pop_matrix();
            }
        }
    }
    render_end();
    glDisable(GL_TEXTURE_2D);
}

void draw_selection(Selection* selection) {
    float x = (sin(num_frames / 100.f * M_PI * 2) + 1) / 2 * 0.2f + 0.6f; // sine between 0.6 and 0.8

    push_matrix(Mtx::translate(selection->pos));

    glColor4f(1.f, 1.f, 1.f, 1.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    render_begin();
    if (selection->pos.y != -1) draw_cube();
    else draw_yplane(Vec2::zero(), Vec2::one(), 1);
    render_end();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glColor4f(1.f, 1.f, 1.f, x);
    render_begin();
    if      (selection->normal == IVec3::pos_x()) draw_xplane(Vec2::zero(), Vec2::one(), 1);
    else if (selection->normal == IVec3::neg_x()) draw_xplane(Vec2::zero(), Vec2::one(), 0);
    else if (selection->normal == IVec3::pos_y()) draw_yplane(Vec2::zero(), Vec2::one(), 1);
    else if (selection->normal == IVec3::neg_y()) draw_yplane(Vec2::zero(), Vec2::one(), 0);
    else if (selection->normal == IVec3::pos_z()) draw_zplane(Vec2::zero(), Vec2::one(), 1);
    else if (selection->normal == IVec3::neg_z()) draw_zplane(Vec2::zero(), Vec2::one(), 0);
    else printf("invalid side: %d %d %d\n", selection->normal.x, selection->normal.y, selection->normal.z);
    render_end();

    pop_matrix();
}