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

#include "gl41_renderer.h"

#include "misc/utility/generator.h"
#include "misc/utility/log.h"

#include <algorithm>

GL41Renderer::GL41Renderer() {

}

GL41Renderer::~GL41Renderer() {
    cleanup();
}

u64 GL41Renderer::add_mesh(struct Mesh& mesh) {
    if(mesh.verticies.empty()) return -1;

    GL41Mesh gl41_mesh(mesh);
    u64 mesh_id = Generator::generate_id();

    gl41_meshes.emplace(mesh_id, std::move(mesh));

    return mesh_id;
}   

u64 GL41Renderer::add_shader(struct Shader& shader) {
    if(!shader.vertex || !shader.fragment) return -1;

    GL41Shader gl41_shader(shader);

    u64 shader_id = Generator::generate_id();

    gl41_shaders.emplace(shader_id, std::move(shader));

    return shader_id;
}

void GL41Renderer::draw_mesh(const u64& mesh_id) {
    auto it =  gl41_meshes.find(mesh_id);
    if(it == gl41_meshes.end()) {
        ERROR("Mesh Not Found: %llu", mesh_id);
        return;    
    }   

    Mesh& mesh = it->second;

    for (size_t i = 0; i < mesh.textures.size(); ++i) {
        u32 texture = mesh.textures[i];

        if (i < bound_textures.size() && bound_textures[i] == texture)
            continue;

        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture);

        if (i < bound_textures.size())
            bound_textures[i] = texture;
        else
            bound_textures.push_back(texture);
    }

    if(bound_vao != mesh.vao) {
        glBindVertexArray(mesh.vao);
        bound_vao = mesh.vao;
    }

    glDrawElements(GL_TRIANGLES, mesh.size, GL_UNSIGNED_INT, 0);
}

void GL41Renderer::use_shader(const u64& shader_id) {
    auto it = gl41_shaders.find(shader_id);
    if(it != gl41_shaders.end()) {
        if(current_shader != shader_id) {
            current_shader = shader_id;
            it->second.use();
        }
    } else {
        ERROR("Shader not found: %llu", shader_id);
        return;
    }
}

void GL41Renderer::submit_draw_call(const struct DrawCall& draw_call) {
    draw_queue.push_back(draw_call);
}

void GL41Renderer::draw() {
    if(draw_queue.empty()) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bound_vao = 0;
    bound_textures.clear();
    current_shader = 0;

    std::sort(draw_queue.begin(), draw_queue.end(),
        [](const DrawCall& a, const DrawCall& b) {
            if(a.shader_id == b.shader_id)
                return a.mesh_id < b.mesh_id;
            return a.shader_id < b.shader_id;
        }
    );

    for(const auto& call : draw_queue) {
        use_shader(call.shader_id);

        auto it_shader = gl41_shaders.find(call.shader_id);
        if(it_shader != gl41_shaders.end()) {
            // it_shader->second.set_mat4("your transform", call.transform);

            for(const auto& pair : call.mat4_uniform) {
                it_shader->second.set_mat4(pair.first.c_str(), pair.second);
            }
        }

        draw_mesh(call.mesh_id);
    }

    draw_queue.clear();
}

void GL41Renderer::cleanup() {
    gl41_shaders.clear();

    for(auto& pair : gl41_meshes) {
        Mesh& mesh = pair.second;
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
        for(u32 tex : mesh.textures) {
            glDeleteTextures(1, &tex);
        }
    }

    gl41_meshes.clear();
}