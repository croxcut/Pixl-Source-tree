#include "Window.h"

struct Window window;

void create(FWindow init, FWindow render, FWindow update, FWindow cleanup) {

    if(!glfwInit()) {
        printf("[Engine]: Failed to Initialize GLFW!");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}   


static void _init() {
    window.init();
}

static void _render() {
    window.render();

}

static void _update() {
    window.update();
}

static void _cleanup() {
    window.cleanup();
}

void loop() {
    _init();

    while(!glfwWindowShouldClose(window.handle)) {
        _update();

        _render();
    }
    
    _cleanup();
}

