#ifndef __IAPPLOGIC__
#define __IAPPLOGIC__

class IAppLogic{

public:

    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;

};

#endif