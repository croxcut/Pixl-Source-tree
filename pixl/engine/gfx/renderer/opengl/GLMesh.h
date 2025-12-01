#ifndef __GL_MESH__
#define __GL_MESH__

#include "../../../util/Util.h"

struct Vertex {
    f32 x, y, z;
    f32 r, g, b;
};

struct GLMesh{
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
};

#endif