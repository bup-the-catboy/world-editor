#include "selection.h"

#include <GL/gl.h>
#include <SDL3/SDL.h>

#include "renderer.h"

#include <stdbool.h>

void mkray(float x, float y, Vec3* pos, Vec3* dir) {
    Mtx mtx = (mtx_projection * mtx_modelview).inv();
    *pos =  (mtx * Vec4(x, y, -1, 1)).divide().vec3();
    *dir = ((mtx * Vec4(x, y,  1, 1)).divide().vec3() - *pos).normalized();
}

bool intersect_aabb(Vec3 origin, Vec3 dir, float* tmin_out, float* tmax_out) {
    float tmin = -INFINITY, tmax = INFINITY;
    for (int i = 0; i < 3; i++) {
        float o = i == 0 ? origin.x : (i == 1 ? origin.y : origin.z);
        float d = i == 0 ? dir.x    : (i == 1 ? dir   .y : dir   .z);
        float min = 0.0f, max = 32.0f;

        if (fabsf(d) < 1e-6f) {
            if (o < min || o > max) return false;
        }
        else {
            float t1 = (min - o) / d;
            float t2 = (max - o) / d;
            if (t1 > t2) {
                float tmp = t1;
                t1 = t2;
                t2 = tmp;
            }
            if (tmin < t1) tmin = t1;
            if (tmax > t2) tmax = t2;
            if (tmin > tmax) return false;
        }
    }

    *tmin_out = tmin;
    *tmax_out = tmax;
    return true;
}

void cast(World world, Vec3 pos, Vec3 dir, Selection* selection) {
    float t = 0.0;

    int ix = floor(pos.x);
    int iy = floor(pos.y);
    int iz = floor(pos.z);

    int stepx = (dir.x > 0) ? 1 : -1;
    int stepy = (dir.y > 0) ? 1 : -1;
    int stepz = (dir.z > 0) ? 1 : -1;

    float tx_delta = fabsf(1 / dir.x);
    float ty_delta = fabsf(1 / dir.y);
    float tz_delta = fabsf(1 / dir.z);

    float xdist = (stepx > 0) ? (ix + 1 - pos.x) : (pos.x - ix);
    float ydist = (stepy > 0) ? (iy + 1 - pos.y) : (pos.y - iy);
    float zdist = (stepz > 0) ? (iz + 1 - pos.z) : (pos.z - iz);

    float tx_max = (tx_delta < INFINITY) ? tx_delta * xdist : INFINITY;
    float ty_max = (ty_delta < INFINITY) ? ty_delta * ydist : INFINITY;
    float tz_max = (tz_delta < INFINITY) ? tz_delta * zdist : INFINITY;

    int stepped_index = -1;

    while (t <= WORLD_SIZE * WORLD_SIZE * WORLD_SIZE) {
        bool is_solid = false;
        if (ix >= 0 && ix < WORLD_SIZE && iz >= 0 && iz < WORLD_SIZE) {
            if (iy >= 0 && iy < WORLD_SIZE) is_solid = world[ix][iy][iz];
            else if (iy == -1) is_solid = true;
        }
        if (is_solid) {
            /*selection->pos = Vec3(
                pos.x + t * dir.x,
                pos.y + t * dir.y,
                pos.z + t * dir.z
            );*/
            selection->pos = IVec3(ix, iy, iz);
            if (stepped_index == 0) selection->normal = IVec3::pos_x() * -stepx;
            if (stepped_index == 1) selection->normal = IVec3::pos_y() * -stepy;
            if (stepped_index == 2) selection->normal = IVec3::pos_z() * -stepz;
            break;
        }
        if (tx_max < ty_max) {
            if (tx_max < tz_max) {
                ix += stepx;
                t = tx_max;
                tx_max += tx_delta;
                stepped_index = 0;
            }
            else {
                iz += stepz;
                t = tz_max;
                tz_max += tz_delta;
                stepped_index = 2;
            }
        }
        else {
            if (ty_max < tz_max) {
                iy += stepy;
                t = ty_max;
                ty_max += ty_delta;
                stepped_index = 1;
            }
            else {
                iz += stepz;
                t = tz_max;
                tz_max += tz_delta;
                stepped_index = 2;
            }
        }
    }
    return;
}

Selection* get_selection(World world, SDL_Window* window) {
    int width, height;
    float mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_GetWindowSizeInPixels(window, &width, &height);
    //mouse_y += 64; // idfk why

    float x = (2 * mouse_x) / width - 1;
    float y = 1 - (2 * mouse_y) / height;

    Vec3 pos, dir;
    Selection* selection = (Selection*)malloc(sizeof(Selection));
    selection->pos = IVec3(15, -1, 15);
    selection->normal = IVec3::pos_y();
    mkray(x, y, &pos, &dir);
    cast(world, pos, dir, selection);
    return selection;
}