#include "OpenGLRenderer.h"

OpenGLRenderer::OpenGLRenderer() {
}

void OpenGLRenderer::init() {

    LOG(INFO, "OpenGL Renderer Initialized!");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW );
    
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(u32), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

void OpenGLRenderer::createMesh(const Mesh& mesh) {
    this->mesh = mesh;
}

void OpenGLRenderer::createShader(const Shader& shader) {
    glShader = new GLShader(shader.vertexSource, shader.fragmentSource);
}

void OpenGLRenderer::update() {

}

void OpenGLRenderer::draw() {
    glShader->use();

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OpenGLRenderer::cleanup() {    
    glShader->cleanup();
    delete glShader;
}