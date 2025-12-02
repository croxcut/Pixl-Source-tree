#ifndef __IRENDERER__
#define __IRENDERER__

#include "Mesh.h"
#include "Shader.h"

class IRenderer{

public:

    virtual ~IRenderer() {}

    virtual void init() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void cleanup() = 0;
    
    // Return IDs for created resources
    virtual std::string createMesh(const Mesh& mesh) = 0;
    virtual std::string createShader(const Shader& shader) = 0;
    
    // Optional: Methods to use specific resources
    virtual void useShader(const std::string& shaderId) = 0;
    virtual void drawMesh(const std::string& meshId) = 0;

};

#endif