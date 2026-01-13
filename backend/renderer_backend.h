#ifndef VENDOR_H
#define VENDOR_H

#include "misc/utility/types.h"

#include <vector>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp> 

struct Vertex{
    f32 x, y, z;
    f32 r, g, b;
    f32 u, v;
};

struct Texture{
    std::vector<const char*> texture_paths;
};

struct Mesh {
    std::vector<Vertex> verticies;
    std::vector<u32> indices;
    std::vector<u32> textures;

    u32 vao;
    u32 vbo;
    u32 ebo;

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
    std::unordered_map<std::string, glm::mat4> mat4_uniform;
};

class Renderer {
public:

    enum class Backend {
        OPENGL = 0x01,
        VULKAN,
        DX3D12,
        METAL      
    };

public:


};

#endif