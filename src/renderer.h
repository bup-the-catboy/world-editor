#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

#include <GL/gl.h>

extern Mtx mtx_projection;
extern Mtx mtx_modelview;
extern GLuint tileset_texture;

void unproject(float x, float y, Vec3* pos, Vec3* dir);
void prepare_rendering(float near_plane = .1f);
void draw_grid();
void draw_voxels(World world);
void draw_selection(Selection* selection);
BlockID draw_block_selection(float x, float y, float off_x, float off_y, BlockID prev);

#endif