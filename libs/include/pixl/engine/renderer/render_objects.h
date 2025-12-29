#ifndef RENDER_OBJECTS_H
#define RENDER_OBJECTS_H

#include "pixl/utility/types.h"

#include <vector>

struct Vertex {
    f32 x, y, z;
};

struct Mesh {
    std::vector<Vertex> vertices; 
};

struct ShaderSource {
    const char* vertex;
    const char* fragment;
};

#endif