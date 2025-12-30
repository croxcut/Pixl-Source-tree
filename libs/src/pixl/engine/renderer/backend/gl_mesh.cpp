#include "pixl/engine/renderer/backend/gl_mesh.h"

#include <pixl/utility/log.h>
#include <glad/glad.h>

GLMesh::GLMesh(struct Mesh& mesh) {

    if(mesh.vertices.empty()) {
        WARN("Vertices are empty : %llu", mesh.vertices.size());
        return;
    }

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);    

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(u32), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(f32)));
    glEnableVertexAttribArray(2);
    
    mesh.size = mesh.vertices.size();
}
