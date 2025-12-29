#include "pixl/engine/renderer/backend/gl_mesh.h"

#include <pixl/utility/log.h>
#include <glad/glad.h>

void GLMesh::create_mesh(struct Mesh& mesh) {

    if(mesh.vertices.empty()) {
        WARN("Vertices are empty : %llu", mesh.vertices.size());
        return;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);    

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    size = mesh.vertices.size();
}

void GLMesh::draw() const {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, size);
}