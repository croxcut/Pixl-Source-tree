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

#ifndef GL41_RENDERER_H
#define GL41_RENDERER_H

#include "backend/pxl_renderer.h"

#include "backend/rendering/gl41_shader.h"
#include "backend/rendering/gl41_mesh.h"

class GL41Renderer : public PXLRenderer {

private:
    
    std::unordered_map<u64, GL41Shader> gl41_shaders;
    std::unordered_map<u64, Mesh> gl41_meshes;

    u32 bound_vao = 0;
    std::vector<u32> bound_textures;
    u64 current_shader = 0;

    std::vector<struct DrawCall> draw_queue;

public:
    GL41Renderer();
    ~GL41Renderer();

    u64 add_mesh(struct Mesh& mesh) override;
    u64 add_shader(struct Shader& shader) override;
    void submit_draw_call(const struct DrawCall& draw_call) override;
    void draw() override;
    void cleanup() override;

private:

    void draw_mesh(const u64& mesh_id);
    void use_shader(const u64& shader_id);

};

#endif