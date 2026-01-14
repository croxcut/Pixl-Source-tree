/*==============================================================================
    PIXL ENGINE

    Copyright (C) 2026 John Paul Valenzuela
    Author : John Paul Valenzuela
    Email  : johnpaulvalenzuelaog@gmail.com

    This file is part of PIXL ENGINE.

    PIXL ENGINE is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PIXL ENGINE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PIXL ENGINE.  If not, see <https://www.gnu.org/licenses/>.
==============================================================================*/

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