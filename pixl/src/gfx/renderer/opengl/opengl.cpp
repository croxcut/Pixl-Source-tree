#define STB_IMAGE_IMPLEMENTATION

#include "pixl/engine/gfx/renderer/opengl/opengl.h"


#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

OpenGL::OpenGL() {
    stbi_set_flip_vertically_on_load(true);
}

OpenGL::~OpenGL() {
    cleanup();
}

void OpenGL::init() {
    glEnable(GL_DEPTH_TEST);
}

void OpenGL::tick() {
}

u64 OpenGL::createMesh(const Mesh& mesh) {
    MeshData meshData;  

    for(const char* path : mesh.texturePaths) {
        s32 width, height, nrChannels;
        unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
        if(!data) {
            LOG(ERROR, "Failed to load texture: %s", path);
            continue;
        }

        u32 texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        u32 format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        meshData.textures.push_back(texId);
    }

    glGenVertexArrays(1, &meshData.vao);
    glBindVertexArray(meshData.vao);

    glGenBuffers(1, &meshData.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &meshData.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(u32), mesh.indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    meshData.indexCount = (u32)(mesh.indices.size());

    u64 meshId = generateId();
    meshes[meshId] = meshData;

    LOG(INFO, "Created mesh with ID: %llu",meshId);
    return meshId;
}

u64 OpenGL::createShader(const Shader& shader) {
    GLShader* glShader = new GLShader(shader.vertexSource, shader.fragmentSource);
    u64 shaderId = generateId();
    shaders[shaderId] = glShader;
    LOG(INFO, "Created Shader with ID: %llu",shaderId);
    return shaderId;
}

void OpenGL::useShader(const u64& shaderId) {
    auto it = shaders.find(shaderId);
    if(it != shaders.end()) {
        if(currentShader != shaderId) {
            currentShader = shaderId;
            it->second->use();
        }
    } else {
        LOG(ERROR, "Shader not found: %llu", shaderId);
        return;
    }
}

void OpenGL::setViewProjection(
    const glm::mat4& view,
    const glm::mat4& projection
) {
    viewMatrix = view;
    projectionMatrix = projection;
}

void OpenGL::drawMesh(const u64& meshId) {
    auto it = meshes.find(meshId);
    if(it == meshes.end()) {
        LOG(ERROR, "Mesh not found: %llu", meshId);
        return;
    }

    MeshData& meshData = it->second;

    for (size_t i = 0; i < meshData.textures.size(); i++) {
        if (boundTextures.size() <= i || boundTextures[i] != meshData.textures[i]) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, meshData.textures[i]);
            if (boundTextures.size() <= i) boundTextures.push_back(meshData.textures[i]);
            else boundTextures[i] = meshData.textures[i];
        }
    }

    if (boundVAO != meshData.vao) {
        glBindVertexArray(meshData.vao);
        boundVAO = meshData.vao;
    }

    glDrawElements(GL_TRIANGLES, meshData.indexCount, GL_UNSIGNED_INT, 0);
}

void OpenGL::submitDrawCall(const u64& meshId, const u64& shaderId, const glm::mat4& transform) {
    drawQueue.push_back({meshId, shaderId, transform});
}

void OpenGL::draw() {

    if (drawQueue.empty()) return;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    boundVAO = 0;
    boundTextures.clear();
    currentShader = 0;

    std::sort(drawQueue.begin(), drawQueue.end(),
        [](const DrawCall& a, const DrawCall& b) {
            if (a.shaderId == b.shaderId)
                return a.meshId < b.meshId;
            return a.shaderId < b.shaderId;
        }
    );

    for (const auto& call : drawQueue) {

        useShader(call.shaderId);
        GLShader* shader = shaders[call.shaderId];

        // 🔑 CAMERA MATRICES (ONCE PER DRAW)
        shader->setMat4("view", viewMatrix);
        shader->setMat4("projection", projectionMatrix);

        // 🔑 PER-OBJECT TRANSFORM
        shader->setMat4("transform", call.transform);

        drawMesh(call.meshId);
    }

    drawQueue.clear();
}

void OpenGL::cleanup() {
    for(auto& pair: shaders) {
        pair.second->cleanup();
        delete pair.second;
    }
    shaders.clear();

    for (auto& pair : meshes) {
        MeshData& meshData = pair.second;
        glDeleteVertexArrays(1, &meshData.vao);
        glDeleteBuffers(1, &meshData.vbo);
        glDeleteBuffers(1, &meshData.ebo);
        for (GLuint tex : meshData.textures) {
            glDeleteTextures(1, &tex);
        }
    }
    meshes.clear();
}