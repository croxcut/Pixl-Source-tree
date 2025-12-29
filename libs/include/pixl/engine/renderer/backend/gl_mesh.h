#ifndef GL_MESH_H
#define GL_MESH_H

#include "../render_objects.h"

class GLMesh {

private:
    u32 vao, vbo;
    size_t size;

public:

    void create_mesh(struct Mesh& mesh);
    void draw() const;
};

#endif