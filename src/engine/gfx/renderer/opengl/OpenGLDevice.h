#ifndef __OPENGL_RENDERER__
#define __OPENGL_RENDERER__

#include "../IGraphics.h"

class OpenGLDevice : public IGraphics{    

public:
    static OpenGLDevice& get();

    void init() override;

    void draw() override;
    
    void createTexture() override; 
    
    void cleanup() override;

};  

#endif