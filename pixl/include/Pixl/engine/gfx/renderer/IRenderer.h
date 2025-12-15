#ifndef __IRENDERER__
#define __IRENDERER__

#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <string>

#include "../../core/Flow.h"

class IRenderer : public Flow{

public:
    virtual ~IRenderer() = default;

    // virtual std::string createMesh(const Mesh& mesh) = 0;
    // virtual std::string createShader(const Shader& shader) = 0;

    // virtual void useShader(const std::string& shaderId) = 0;
    // virtual void drawMesh(const std::string& meshId) = 0;

    virtual u64 createMesh(const Mesh& mesh) = 0;
    virtual u64 createShader(const Shader& shader) = 0;

    virtual void drawMesh(const u64& meshId) = 0;
    virtual void useShader(const u64& shaderId) = 0;

    virtual void submitDrawCall(const u64& meshId, const u64& shaderId, const glm::mat4& transform = glm::mat4(1.0f)) = 0;

};

#endif
