#include "types.h"

#include <GL/gl.h>
#include <stdio.h>

#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SCALE 6

static int num_frames = 0;

template<typename T> void swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

enum Flip {
    Flip_None,
    Flip_XAxis,
    Flip_YAxis
};

enum Rotation {
    deg0,
    deg90,
    deg180,
    deg270
};

struct Vertex {
    Vec3 xyz;
    Vec2 uv;
};

struct Texture {
    IVec4 src;
    Flip flip;
    Rotation rot;

    Vec2 uv[4];

    Texture(IVec4 src = IVec4(), Rotation rot = deg0, Flip flip = Flip_None): src(src), flip(flip), rot(rot) {
        Vec4 vec = Vec4(
            (src.x        ) / (float)TILEMAP_WIDTH,
            (src.y        ) / (float)TILEMAP_HEIGHT,
            (src.x + src.z) / (float)TILEMAP_WIDTH,
            (src.y + src.w) / (float)TILEMAP_HEIGHT
        );
        uv[0] = Vec2(vec.z, vec.w);
        uv[1] = Vec2(vec.x, vec.w);
        uv[2] = Vec2(vec.x, vec.y);
        uv[3] = Vec2(vec.z, vec.y);

        switch (flip) {
            case Flip_None: break;
            case Flip_XAxis:
                swap(uv[2], uv[3]);
                swap(uv[0], uv[1]);
                break;
            case Flip_YAxis:
                swap(uv[1], uv[2]);
                swap(uv[0], uv[3]);
                break;
        }
        for (int i = 0; i < rot; i++) {
            swap(uv[2], uv[3]);
            swap(uv[1], uv[2]);
            swap(uv[0], uv[1]);
        }
    }
};

struct {
    Vec3 pos = Vec3(0, 8, 0);
    Vec3 rot = Vec3(-25, 180+45, 0);
} camera;

Mtx mtx_projection = Mtx::identity();
Mtx mtx_modelview  = Mtx::identity();
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

void unproject(float x, float y, Vec3* pos, Vec3* dir) {
    Mtx mtx = (mtx_projection * mtx_modelview).inv();
    *pos =  (mtx * Vec4(x, y, -1, 1)).divide().vec3();
    *dir = ((mtx * Vec4(x, y,  1, 1)).divide().vec3() - *pos).normalized();
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

    //mtx_projection = Mtx::perspective(70, 3/2.f, .1f, 100.f);
    mtx_projection = Mtx::orthographic(-SCALE, SCALE, -SCALE * 2/3.f, SCALE * 2/3.f, .1f, 100.f);
    mtx_modelview = Mtx::identity()
        * Mtx::roll (-camera.rot.z * Angle::rad)
        * Mtx::pitch(-camera.rot.x * Angle::rad)
        * Mtx::yaw  (-camera.rot.y * Angle::rad)
        * Mtx::translate(-camera.pos)
    ;

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

#define SLICE(x) x, x
#define SIDES(x) x, x, x, x
#define FACES(x) x, x, x, x, x, x

void draw_xplane(Vec2 from, Vec2 to, float x, Texture tex = Texture()) {
    put_vertex(x, from.x, from.y, tex.uv[1].x, tex.uv[1].y);
    put_vertex(x,  to .x, from.y, tex.uv[2].x, tex.uv[2].y);
    put_vertex(x,  to .x,  to .y, tex.uv[3].x, tex.uv[3].y);
    put_vertex(x, from.x,  to .y, tex.uv[0].x, tex.uv[0].y);
}

void draw_yplane(Vec2 from, Vec2 to, float y, Texture tex = Texture()) {
    put_vertex(from.x, y, from.y, tex.uv[0].x, tex.uv[0].y);
    put_vertex( to .x, y, from.y, tex.uv[1].x, tex.uv[1].y);
    put_vertex( to .x, y,  to .y, tex.uv[2].x, tex.uv[2].y);
    put_vertex(from.x, y,  to .y, tex.uv[3].x, tex.uv[3].y);
}

void draw_zplane(Vec2 from, Vec2 to, float z, Texture tex = Texture()) {
    put_vertex(from.x, from.y, z, tex.uv[0].x, tex.uv[0].y);
    put_vertex( to .x, from.y, z, tex.uv[1].x, tex.uv[1].y);
    put_vertex( to .x,  to .y, z, tex.uv[2].x, tex.uv[2].y);
    put_vertex(from.x,  to .y, z, tex.uv[3].x, tex.uv[3].y);
}

void draw_box(Vec3 from, Vec3 to, Texture posy = Texture(), Texture negy = Texture(), Texture posx = Texture(), Texture negx = Texture(), Texture posz = Texture(), Texture negz = Texture()) {
    draw_xplane(Vec2(from.y, from.z), Vec2(to.y, to.z), from.x, negx);
    draw_xplane(Vec2(from.y, from.z), Vec2(to.y, to.z),  to .x, posx);
    draw_yplane(Vec2(from.x, from.z), Vec2(to.x, to.z), from.y, negy);
    draw_yplane(Vec2(from.x, from.z), Vec2(to.x, to.z),  to .y, posy);
    draw_zplane(Vec2(from.x, from.y), Vec2(to.x, to.y), from.z, negz);
    draw_zplane(Vec2(from.x, from.y), Vec2(to.x, to.y),  to .z, posz);
}

void draw_cube(Texture posy = Texture(), Texture negy = Texture(), Texture posx = Texture(), Texture negx = Texture(), Texture posz = Texture(), Texture negz = Texture()) {
    draw_box(Vec3(0, 0, 0), Vec3(1, 1, 1), posy, negy, posx, negx, posz, negz);
}

void draw_block(BlockID block) {
    switch (block) {
        case Block_Air: break;
        case Block_Dirt:          draw_cube(FACES(IVec4(32, 0, 16, 16))); break;
        case Block_DirtWall:      draw_cube(SLICE(IVec4(32, 0, 16, 16)), SIDES(IVec4(48, 0, 16, 16))); break;
        case Block_Ground:        draw_cube(IVec4(0, 0, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_PathStraight1: draw_cube(IVec4(32, 16, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_PathStraight2: draw_cube(IVec4(32, 32, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_PathCurved1:   draw_cube(IVec4(0, 16, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_PathCurved2:   draw_cube(IVec4(16, 16, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_PathCurved3:   draw_cube(IVec4(0, 32, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_PathCurved4:   draw_cube(IVec4(16, 32, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_Intersection:  draw_cube(IVec4(48, 32, 16, 16), IVec4(32, 0, 16, 16), SIDES(IVec4(16, 0, 16, 16))); break;
        case Block_Bridge1: draw_box(Vec3(0, 0.5, 0), Vec3(1, 1, 1), SLICE(Texture(IVec4(0, 48, 16, 16), deg90)), SLICE(IVec4(16, 48, 16, 8)), SLICE(IVec4(16, 56, 16, 8))); break;
        case Block_Bridge2: draw_box(Vec3(0, 0.5, 0), Vec3(1, 1, 1), SLICE(Texture(IVec4(0, 48, 16, 16), deg0 )), SLICE(IVec4(16, 56, 16, 8)), SLICE(IVec4(16, 48, 16, 8))); break;
        case Block_Water:
            draw_yplane(Vec2(0, 0), Vec2(1, 1), 0, IVec4(48, 16, 16, 16));
            draw_yplane(Vec2(0, 0), Vec2(1, 1), 0.125, IVec4(64, 0, 16, 16));
            break;
        default: break;
    }
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
                draw_block(world[x][y][z]);
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

BlockID draw_block_selection(float x, float y, float off_x, float off_y, BlockID prev) {
    glDisable(GL_DEPTH_TEST);
    glColor4f(0.f, 0.f, 0.f, .5f);
    render_begin();
    glVertex2f(-1, -1);
    glVertex2f( 1, -1);
    glVertex2f( 1,  1);
    glVertex2f(-1,  1);
    render_end();
    glEnable(GL_DEPTH_TEST);

    glClear(GL_DEPTH_BUFFER_BIT);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    render_begin();

    x -= 12;
    y += 24;

    int selected = 0;
    int num_items = Block_Count - Block_Start;
    float angle_step = 2*M_PI / num_items;
    float angle = 0;

    float distance = sqrt(off_x * off_x + off_y * off_y);
    float select_angle = atan2f(off_y, off_x) + angle_step / 2;
    if (select_angle < 0)      select_angle += 2*M_PI;
    if (select_angle > 2*M_PI) select_angle -= 2*M_PI;
    int select_block = distance > 48.f && distance < 192.f ? (int)(select_angle / (2*M_PI) * num_items) + Block_Start : 0;

    for (int i = Block_Start; i < Block_Count; i++) {
        float scale = SCALE * 2;
        if (select_block == i) scale = SCALE * 1.5f;
        mtx_projection = Mtx::orthographic(-scale, scale, -scale * 2/3.f, scale * 2/3.f, .1f, 100.f);

        float ox = cos(angle) * 128;
        float oy = sin(angle) * 128;
        float nx = (2 * (x + ox)) / 768 - 1;
        float ny = 1 - (2 * (y + oy)) / 512;

        Vec3 pos, dir;
        unproject(nx, ny, &pos, &dir);
        pos += dir * 2;

        push_matrix(Mtx::translate(pos));
        draw_block((BlockID)i);
        pop_matrix();

        angle += angle_step;
    }

    render_end();
    glDisable(GL_TEXTURE_2D);

    return select_block == Block_Air ? prev : (BlockID)select_block;
}