#ifndef __MESH_DATA__
#define __MESH_DATA__

#include "../../util/Types.h"

struct MeshData {
    u32 vao, vbo, ebo;
    u32 indexCount;
    std::vector<u32> textures;
};

#endif