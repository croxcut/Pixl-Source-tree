#ifndef __IGRAPHICS_DEVICE__
#define __IGRPAHICS_DEVICE__

class IGraphicsDevice
{
public:
    
    virtual ~IGraphicsDevice() = default;

    virtual void init() = 0;

    virtual void createTexture() = 0;

    virtual void draw() = 0;
    virtual void cleanup() = 0;

};


#endif