#ifndef __OPENGL_RENDERER__
#define __OPENGL_RENDERER__

#include "../IRenderer.h"
#include "shader/GLShader.h"
#include "GLMesh.h"

class OpenGLRenderer : public IRenderer {

private:
    GLShader* shader;
    GLMesh* mesh;

    u32 vao, vbo, ebo;
    
public:

    OpenGLRenderer(
        GLShader *gl_shader,
        GLMesh *gl_mesh
    );

    void init() override;
    void update() override;
    void draw() override;
    void cleanup() override;

};

#endif