#ifndef __OPENGL_RENDERER__
#define __OPENGL_RENDERER__

#include "../IRenderer.h"
#include "shader/GLShader.h"
#include "../../../util/Log.h"

class OpenGLRenderer : public IRenderer {

private:
    GLShader* glShader;
    Mesh mesh;
    Shader shader;

    u32 vao, vbo, ebo;
    
public:

    OpenGLRenderer();

    void init() override;
    void update() override;
    void draw() override;
    void cleanup() override;

    void createMesh(const Mesh& mesh) override;
    void createShader(const Shader& Shader) override;

};

#endif