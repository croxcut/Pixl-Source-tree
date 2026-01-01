#ifndef KEYBINDS_H
#define KEYBINDS_H

#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <pixl/window/window.h>

namespace input {
    static Window* _window = nullptr;

    inline void init(Window* window) { _window = window; }

    inline bool key_down(int key) {
        if (!_window) return false;
        return glfwGetKey(_window->get_handle(), key) == GLFW_PRESS;
    }
};
#endif