#ifndef ENGINE_H
#define ENGINE_H

#include "misc/utility/types.h"
#include "core/window/window.h"
#include "scene/iapplogic.h"

class Engine {

private:
    Window* window = nullptr;
    IAppLogic* applogic = nullptr;

public:
    Engine(IAppLogic& applogic);
    ~Engine();

    void start();
    void stop();

private:

    void run();
    void init();
    void tick(const f32& dt);
    void render();
    void cleanup();
};

#endif