#ifndef ENGINE_H
#define ENGINE_H

#include <pixl/window/window.h>
#include "i_applogic.h"


class Engine {

private: 
    IAppLogic* applogic = nullptr;
    Window* window = nullptr;
    
    bool running = false;
    
public:
    Engine(IAppLogic& applogic);
    ~Engine();

    void start();
    void stop();

private:
    void init();
    void run();
    void tick(const f32& dt);
    void render();
    void cleanup();
};

#endif

