#ifndef RENDERER_H
#define RENDERER_H

#include <pixl/engine/renderer/backend/gl_mesh.h>
#include <pixl/engine/renderer/backend/gl_shader.h>

class Renderer {
public:
    virtual void add_mesh(struct Mesh& mesh) = 0;
    virtual void add_shader(struct Shader& shader) = 0;
    virtual void draw() = 0;
    virtual void cleanup() = 0;

};

#endif