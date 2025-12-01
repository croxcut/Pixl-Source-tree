#ifndef __WINDOW__
#define __WINDOW__

#include "util/Util.h"

#include "gfx/IAppLogic.h"
#include "util/Log.h"

class Window{

private:
    GLFWwindow* handle;
    IAppLogic& appLogic;

private:
    Window(IAppLogic& logic);
    void _init();
    void _tick();
    void _render();
    void _cleanup();

public:
    
    static Window& createWindow(IAppLogic& logic);
    GLFWwindow* getWindow();

    void start();
    void cleanup();

    ~Window();

};

#endif