#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "pixl/utility/log.h"
#include "pixl/utility/types.h"

struct WindowOpts {
    u32 width = 1280;
    u32 height = 720;
    const char* title = "Pixl Test";
};

class Window {

private:
    GLFWwindow* handle = nullptr;

    bool vsync = false;

public:
    ~Window();

    static Window* create_window(struct WindowOpts& opts);

    void refresh();
    void poll_events();

    bool close();

    void toggle_vsync();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
private:
    Window(struct WindowOpts& opts);
    void cleanup();
};

#endif