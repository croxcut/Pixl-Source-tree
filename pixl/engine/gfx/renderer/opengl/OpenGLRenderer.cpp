#include "OpenGLRenderer.h"

#include <random>

OpenGLRenderer::OpenGLRenderer() {
}

OpenGLRenderer::~OpenGLRenderer() {
    cleanup();
}

std::string generateId() {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    
    std::string id = "ID_";
    for (int i = 0; i < 8; ++i) {
        id += alphanum[dis(gen)];
    }
    return id;
}

void OpenGLRenderer::init() {

}

std::string OpenGLRenderer::createMesh(const Mesh& mesh) {
    LOG(INFO, "Creating Mesh ID...");    

    MeshData meshData;
    
    glGenVertexArrays(1, &meshData.vao);
    glBindVertexArray(meshData.vao);

    glGenBuffers(1, &meshData.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), 
                 mesh.vertices.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &meshData.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(u32), 
                 mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    meshData.indexCount = mesh.indices.size();
    
    std::string meshId = generateId();
    meshes[meshId] = meshData;
    
    LOG(INFO, "Created mesh with ID: %s", meshId.c_str());
    return meshId;
}

std::string OpenGLRenderer::createShader(const Shader& shader) {
    GLShader* glShader = new GLShader(shader.vertexSource, shader.fragmentSource);
    std::string shaderId = generateId();
    shaders[shaderId] = glShader;
    
    LOG(INFO, "Created shader with ID: %s", shaderId.c_str());
    return shaderId;
}
void OpenGLRenderer::update() {

}

void OpenGLRenderer::useShader(const std::string& shaderId) {
    auto it = shaders.find(shaderId);
    if (it != shaders.end()) {
        currentShader = shaderId;
        it->second->use();
    } else {
        LOG(ERROR, "Shader not found: %s", shaderId.c_str());
    }
}

void OpenGLRenderer::drawMesh(const std::string& meshId) {
    auto it = meshes.find(meshId);
    if (it != meshes.end()) {
        MeshData& meshData = it->second;
        glBindVertexArray(meshData.vao);
        glDrawElements(GL_TRIANGLES, meshData.indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    } else {
        LOG(ERROR, "Mesh not found: %s", meshId.c_str());
    }
}

void OpenGLRenderer::draw() {
    if (!currentShader.empty() && !currentMesh.empty()) {
        useShader(currentShader);
        drawMesh(currentMesh);
    }
}

void OpenGLRenderer::cleanup() {
    for (auto& pair : shaders) {
        pair.second->cleanup();
        delete pair.second;
    }
    shaders.clear();
    
    for (auto& pair : meshes) {
        MeshData& meshData = pair.second;
        glDeleteVertexArrays(1, &meshData.vao);
        glDeleteBuffers(1, &meshData.vbo);
        glDeleteBuffers(1, &meshData.ebo);
    }
    meshes.clear();
}