#ifndef __OPENGL_RENDERER__
#define __OPENGL_RENDERER__

#include "../IGraphics.h"
#include "../../../util/Util.h"
#include "../../../util/Log.h"

class OpenGLDevice : public IGraphics{    

private:

    u32 VBO, VAO, EBO;
    u32 vertexShader, fragmentShader, shaderProgram;

public:
    static OpenGLDevice& get();

    void init() override;

    void draw() override;
    
    void createTexture() override; 
    
    void cleanup() override;

private:

    OpenGLDevice();
    // ~OpenGLDevice() = default;
};  

#endif