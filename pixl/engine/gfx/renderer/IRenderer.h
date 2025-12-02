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

    virtual void createMesh(const Mesh& mesh) = 0;
    virtual void createShader(const Shader& shader) = 0;

};

#endif