#ifndef OPENGL_H
#define OPENGL_H

#include <pixl/engine/renderer/backend/gl_mesh.h>
#include <pixl/engine/renderer/backend/gl_shader.h>
#include "../renderer.h"

#include <unordered_map>

class OpenGL : public Renderer {
    std::unordered_map<u64, GLShader> shaders;
    std::unordered_map<u64, GLMesh> meshes;

private:

public:
    OpenGL();
    ~OpenGL();

    void add_mesh(struct Mesh& mesh) override;
    void add_shader(struct Shader& shader) override;
    void draw() override;
    void cleanup() override;

private:

    void draw_mesh();

};

#endif