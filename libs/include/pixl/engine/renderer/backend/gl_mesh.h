#ifndef GL_MESH_H
#define GL_MESH_H

#include "../render_objects.h"

class GLMesh {

private:
    u32 vao, vbo;
    size_t size;

    u32 get_vao() const { return vao; }
    u32 get_vbo() const { return vbo; }
    u32 get_size() const { return size; }

public:
    void create_mesh(struct Mesh& mesh);
    void draw() const;
};

#endif