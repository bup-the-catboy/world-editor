#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

extern Mtx mtx_projection;
extern Mtx mtx_modelview;

void unproject(float x, float y, Vec3* pos, Vec3* dir);
void prepare_rendering();
void draw_grid();
void draw_voxels(World world);
void draw_selection(Selection* selection);
BlockID draw_block_selection(float x, float y, float off_x, float off_y, BlockID prev);

#endif