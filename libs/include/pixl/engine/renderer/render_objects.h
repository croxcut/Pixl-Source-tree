#ifndef RENDER_OBJECTS_H
#define RENDER_OBJECTS_H

#include "pixl/utility/types.h"
#include <glm/glm.hpp>

#include <vector>
#include <unordered_map>
#include <string> 

struct Vertex {
    f32 x, y, z;
    f32 r, g, b;
    f32 u, v;
};

struct Texture {
    std::vector<const char*> texture_paths;
};

struct Mesh {
    std::vector<Vertex> vertices; 
    std::vector<u32> indices;
    std::vector<u32> textures;

    u32 vao, vbo, ebo;
    size_t size;
};

struct Shader {
    const char* vertex;
    const char* fragment;
};

struct DrawCall {
    u64 mesh_id;
    u64 shader_id;
    glm::mat4 transform; 
    std::unordered_map<std::string, glm::mat4> mat4_uniforms; 
};

#endif
