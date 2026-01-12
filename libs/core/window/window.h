#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../config.h"

class Window {

private:
    GLFWwindow* _handle = nullptr;
    struct WindowConfig _config;
    
    bool vsync = false;

public:
    ~Window();

    static Window* create_window();

    void init();
    void setup_window_config(const struct WindowConfig& config);
    void poll_events();
    void refresh();
    void toggle_vsync();
    bool close();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

private:
    Window();
    void cleanup();
    
};

#endif