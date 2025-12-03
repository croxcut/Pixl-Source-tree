#ifndef __OPENGL_RENDERER__
#define __OPENGL_RENDERER__

#include "../IRenderer.h"
#include "../../shader/GLShader.h"
#include "../../../util/Log.h"
#include "../MeshData.h"
#include "../../../util/stb_image.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

class OpenGLRenderer : public IRenderer {

private:
    std::unordered_map<std::string, GLShader*> shaders;
    std::unordered_map<std::string, MeshData> meshes;

    std::string currentShader;
    GLuint boundVAO = 0;
    std::vector<GLuint> boundTextures;

    struct DrawCall {
        std::string meshId;
        std::string shaderId;
        glm::mat4 transform;
    };
    std::vector<DrawCall> drawQueue;

    unsigned int nextId = 1; 
    std::string generateId(const std::string& prefix = "ID_") {
        return prefix + std::to_string(nextId++);
    }

public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void init() override;
    void tick() override;
    void draw() override;
    void cleanup() override;

    std::string createMesh(const Mesh& mesh) override;
    std::string createShader(const Shader& shader) override;

    void useShader(const std::string& shaderId) override;
    void drawMesh(const std::string& meshId) override;

    void submitDrawCall(const std::string& meshId, const std::string& shaderId, const glm::mat4& transform = glm::mat4(1.0f));
};

#endif
