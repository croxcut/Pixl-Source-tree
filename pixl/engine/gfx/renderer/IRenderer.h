#ifndef __IRENDERER__
#define __IRENDERER__

class IRenderer{

public:
    virtual ~IRenderer() {}
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void cleanup() = 0;
};

#endif