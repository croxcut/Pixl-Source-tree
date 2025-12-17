#ifndef __FLOW__
#define __FLOW__

// Engine Flow
class Flow{
public:

    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void draw() = 0;
    virtual void cleanup() = 0;
        
};

#endif