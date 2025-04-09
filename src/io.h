#ifndef IO_H
#define IO_H

#include "types.h"

#include <GL/gl.h>

void read_project(World world);
void write_project(World world);
void export_project(World world);
void read_tileset(GLuint* texture);

#endif