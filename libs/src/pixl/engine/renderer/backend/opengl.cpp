#include "pixl/engine/renderer/backend/opengl.h"

#include "pixl/utility/handles.h"
#include "pixl/utility/log.h"
#include <glad/glad.h>
#include <algorithm>

OpenGL::OpenGL() {

}

OpenGL::~OpenGL() {
    cleanup();
}

u64 OpenGL::add_mesh(struct Mesh& mesh) {
    if (mesh.vertices.empty()) return -1; 

    GLMesh gl_mesh(mesh);

    u64 mesh_id = generate_id();

    meshes.emplace(mesh_id, std::move(mesh));

    return mesh_id;
}

u64 OpenGL::add_shader(struct Shader& shader) {
    if(!shader.fragment || !shader.vertex) return -1;

    GLShader gl_shader(shader);

    u64 shader_id = generate_id();

    shaders.emplace(shader_id, std::move(gl_shader));

    return shader_id;
}

void OpenGL::draw_mesh(const u64& mesh_id) {
    auto it = meshes.find(mesh_id);
    if(it == meshes.end()) {
        ERROR("Mesh Not Found: %llu", mesh_id);
        return;
    }

    Mesh& mesh = it->second;

    for(size_t i = 0; i < mesh.textures.size(); i++) {
        if(bound_textures.size() <= i || bound_textures[i] != mesh.textures[i]) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, mesh.textures[i]);
            if(bound_textures.size() <= i) bound_textures.push_back(mesh.textures[i]);
            else bound_textures[i] = mesh.textures[i];           
        }
    }

    if(bound_vao != mesh.vao) {
        glBindVertexArray(mesh.vao);
        bound_vao = mesh.vao;
    }

    glDrawElements(GL_TRIANGLES, mesh.size, GL_UNSIGNED_INT, 0);
}

void OpenGL::use_shader(const u64& shader_id) {
    auto it = shaders.find(shader_id);
    if(it != shaders.end()) {
        if(current_shader != shader_id) {
            current_shader = shader_id;
            it->second.use();
        }
    } else {
        ERROR("Shader not found: %llu", shader_id);
        return;
    }
}

void OpenGL::submit_draw_call(const DrawCall& draw_call) {
    draw_queue.push_back(draw_call);
}

void OpenGL::draw() {
    if(draw_queue.empty()) return;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bound_vao = 0;
    bound_textures.clear();
    current_shader = 0;

    std::sort(draw_queue.begin(), draw_queue.end(),
        [](const DrawCall a, const DrawCall b) {
            if(a.shader_id == b.shader_id)
                return a.mesh_id < b.mesh_id;
            return a.shader_id < b.shader_id; 
        }
    ); 

    for(const auto& call : draw_queue) {
        use_shader(call.shader_id);
        

        draw_mesh(call.mesh_id);
    }

    draw_queue.clear();
}

void OpenGL::cleanup() {
    shaders.clear();

    for(auto& pair : meshes) {
        Mesh& mesh = pair.second;
        glDeleteVertexArrays(1, &mesh.vao);
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
        for(u32 tex : mesh.textures) {
            glDeleteTextures(1, &tex);
        }
    }
    meshes.clear();
}