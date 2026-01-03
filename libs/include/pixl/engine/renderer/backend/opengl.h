#ifndef OPENGL_H
#define OPENGL_H

#include <pixl/engine/renderer/backend/gl_mesh.h>
#include <pixl/engine/renderer/backend/gl_shader.h>
#include "../i_renderer.h"

#include <unordered_map>

class OpenGL : public IRenderer {
    
private:
    std::unordered_map<u64, GLShader> shaders;
    std::unordered_map<u64, Mesh> meshes;

    u32 bound_vao = 0;
    std::vector<u32> bound_textures;
    u64 current_shader;

    std::vector<DrawCall> draw_queue;

public:
    OpenGL();
    ~OpenGL();

    u64 add_mesh(struct Mesh& mesh) override;
    u64 add_shader(struct Shader& shader) override;
    void draw() override;
    void cleanup() override;
    void submit_draw_call(const DrawCall& draw_call);

private:

    void draw_mesh(const u64& mesh_id);
    void use_shader(const u64& shader_id);

};

#endif