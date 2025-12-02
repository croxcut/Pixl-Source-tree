#ifndef __OPENGL_RENDERER__
#define __OPENGL_RENDERER__

#include "../IRenderer.h"
#include "shader/GLShader.h"
#include "../../../util/Log.h"
#include "../MeshData.h"

class OpenGLRenderer : public IRenderer {

private:
    std::unordered_map<std::string, GLShader*> shaders;
    
    std::unordered_map<std::string, MeshData> meshes;
    
    std::string currentShader;
    std::string currentMesh;
    
    struct DrawCall {
        std::string meshId;
        std::string shaderId;
        glm::mat4 transform;
    };
    std::vector<DrawCall> drawQueue;
    
public:

    OpenGLRenderer();
    ~OpenGLRenderer();

    void init() override;
    void update() override;
    void draw() override;
    void cleanup() override;

    std::string createMesh(const Mesh& mesh) override;
    std::string createShader(const Shader& shader) override;
    
    void useShader(const std::string& shaderId);
    void drawMesh(const std::string& meshId);

        void submitDrawCall(const std::string& meshId, 
                       const std::string& shaderId,
                       const glm::mat4& transform = glm::mat4(1.0f));
};

#endif