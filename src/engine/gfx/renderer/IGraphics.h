#ifndef __IGRAPHICS__
#define __IGRAPHICS__

class IGraphics
{
public:
    
    virtual ~IGraphics() = default;

    virtual void init() = 0;

    virtual void createTexture() = 0;

    virtual void draw() = 0;
    virtual void cleanup() = 0;

};

#endif