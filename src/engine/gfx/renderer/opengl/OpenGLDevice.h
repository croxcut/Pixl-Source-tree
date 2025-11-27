#ifndef __OPENGL_RENDERER__
#define __OPENGL_RENDERER__

#include "IGraphicsDevice.h"

class OpenGLDevice : public IGraphicsDevice{
    
    void init() override;

    void draw() override;

    void cleanup() override;

};  

#endif