#include "pixl/engine/renderer/backend/opengl.h"

#include "pixl/utility/handles.h"

OpenGL::OpenGL() {

}

OpenGL::~OpenGL() {

}

void OpenGL::add_mesh(struct Mesh& mesh) {
    if (mesh.vertices.empty()) return; 

    GLMesh gl_mesh;
    gl_mesh.create_mesh(mesh);

    u64 mesh_id = generate_id();

    meshes.emplace(mesh_id, std::move(gl_mesh));
}

void OpenGL::add_shader(struct Shader& shader) {
    if(!shader.fragment || !shader.vertex) return;

    GLShader gl_shader;
    gl_shader.create_shader(shader);

    u64 shader_id = generate_id();

    shaders.emplace(shader_id, std::move(gl_shader));
}

void OpenGL::draw() {

}

void OpenGL::cleanup() {

}