#ifndef __MAIN_SCENE__
#define __MAIN_SCENE__

#include <pixl/engine/gfx/scene/Scene.h>

class MainScene : public Scene {

    void init() override;
    void tick() override;
    void draw() override;
    void cleanup() override;

};

#endif
