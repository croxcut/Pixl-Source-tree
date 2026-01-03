#ifndef RENDERER_H
#define RENDERER_H

#include <pixl/engine/renderer/backend/gl_mesh.h>
#include <pixl/engine/renderer/backend/gl_shader.h>

class IRenderer {
public:
    virtual u64 add_mesh(struct Mesh& mesh) = 0;
    virtual u64 add_shader(struct Shader& shader) = 0;
    virtual void submit_draw_call(const DrawCall& draw_call);
    virtual void draw() = 0;
    virtual void cleanup() = 0;

};

#endif