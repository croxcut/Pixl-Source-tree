/*********************************************************************************
*                                                                                *
*                                PIXL ENGINE                                     *
*                                                                                *
*  Copyright (c) 2025-present John Paul Valenzuela                               *
*                                                                                *
*  MIT License                                                                   *
*                                                                                *
*  Permission is hereby granted, free of charge, to any person obtaining a copy  *
*  of this software and associated documentation files (the "Software"), to      *
*  deal in the Software without restriction, including without limitation the    *
*  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or   *
*  sell copies of the Software, and to permit persons to whom the Software is    *
*  furnished to do so, subject to the following conditions:                      *
*                                                                                *
*  The above copyright notice and this permission notice shall be included in    *
*  all copies or substantial portions of the Software.                           *
*                                                                                *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL       *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER    *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
*  THE SOFTWARE.                                                                 *
*                                                                                * 
**********************************************************************************/

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

enum class Backend {
    OPENGL = 0x01,
    VULKAN,
    DX3D12,
    METAL      
};

class RendererBackend {
public:
    RendererBackend();
    ~RendererBackend();

};

#endif