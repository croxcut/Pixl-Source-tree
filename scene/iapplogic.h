#ifndef IAPPLOGIC_H
#define IAPPLOGIC_H

#include "misc/utility/types.h"

class IAppLogic {
public:
    virtual void init() = 0;
    virtual void tick(const f32& dt) = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
};

#endif