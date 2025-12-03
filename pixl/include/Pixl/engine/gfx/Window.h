#ifndef __WINDOW__
#define __WINDOW__


#include "../core/IAppLogic.h"
#include "../util/Log.h"
#include "../util/Util.h"

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