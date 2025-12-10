#ifndef __MAIN_SCENE__
#define __MAIN_SCENE__

#include <pixl/engine/gfx/scene/Scene.h>

class MainScene : Scene {

    void init() override;
    void tick() override;
    void render() override;
    void cleanup() override;

};

#endif
