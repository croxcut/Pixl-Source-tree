#ifndef __OPENGL__
#define __OPENGL__

#include <unordered_map>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "../IRenderer.h"
#include "../../shader/GLShader.h"
#include "../../../util/Log.h"
#include "../MeshData.h"
#include "../../../util/stb_image.h"
#include "../../../util/handles/handles.h"


struct DrawCall {
    u64 meshId;
    u64 shaderId;
    glm::mat4 transform;
};

class OpenGL : public IRenderer{

private:
    std::unordered_map<u64, GLShader*> shaders;
    std::unordered_map<u64, MeshData> meshes;

    u32 boundVAO = 0;
    std::vector<u32> boundTextures;
    u64 currentShader;

    std::vector<DrawCall> drawQueue;

public:
    OpenGL();
    ~OpenGL();
    
    void init() override;
    void tick() override;
    void draw() override;
    void cleanup() override;

    u64 createMesh(const Mesh& mesh) override;
    u64 createShader(const Shader& shader) override;

    void drawMesh(const u64& meshId) override;
    void useShader(const u64& shaderId) override;
    
    void submitDrawCall(const u64& meshId, const u64& shaderId, const glm::mat4& transform = glm::mat4(1.0f));

private:

};

#endif