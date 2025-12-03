#ifndef __MESH__
#define __MESH__

#include "../../util/Util.h"

struct Vertex {
    f32 x, y, z;
    f32 r, g, b;
    f32 u, v;
};

struct Mesh{
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<const char*> texturePaths;
};

#endif