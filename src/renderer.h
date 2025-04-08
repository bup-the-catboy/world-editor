#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

extern Mtx mtx_projection;
extern Mtx mtx_modelview;

void prepare_rendering();
void draw_grid();
void draw_voxels(World world);
void draw_selection(Selection* selection);

#endif