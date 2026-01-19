/*********************************************************************************
*                                                                                *
*                                PIXL ENGINE                                     *
*                                                                                *
*  Copyright (c) 2025-present John Paul Valenzuela                               *
*                                                                                *
*  MIT License                                                                   *
*                                                                                *
*  Permission is hereby granted, free of charge, to any person obtaining a copy  *
*  of this software and associated documentation files (the "Software"), to      *
*  deal in the Software without restriction, including without limitation the    *
*  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or   *
*  sell copies of the Software, and to permit persons to whom the Software is    *
*  furnished to do so, subject to the following conditions:                      *
*                                                                                *
*  The above copyright notice and this permission notice shall be included in    *
*  all copies or substantial portions of the Software.                           *
*                                                                                *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL       *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER    *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
*  THE SOFTWARE.                                                                 *
*                                                                                * 
**********************************************************************************/

#include "window.h"

#include "misc/utility/log.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  

Window* Window::create_window() {
    static Window _self;
    return &_self;
}

void Window::init() {

    LOG("Creating Window!");

    if(!glfwInit()) ERROR("Failed to create Window!");

    // OpenGL 4.1 for cross platform compat :)* no posix systems for now can't test them
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _handle = glfwCreateWindow(
        _config.width,
        _config.height,
        _config.title,
        nullptr,
        nullptr
    );

    if(!_handle) {
        ERROR("Failed to create Window");
        cleanup();
        return;
    }

    glfwMakeContextCurrent(_handle);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG("Failed to load GLAD!");
        cleanup();
        return;
    }    

    glfwSetFramebufferSizeCallback(_handle, framebuffer_size_callback);

    glfwSwapInterval(0);

    LOG("Window Created");
}

void Window::setup_window_config(const struct WindowConfig& config) {
    _config = config;
}

void Window::toggle_vsync() {
    vsync = !vsync;
    glfwSwapInterval(vsync);
}

bool Window::close() {
    return _handle ? glfwWindowShouldClose(_handle) : true;
}

void Window::poll_events() {
    glfwPollEvents();
}

void Window::refresh() {
    glfwSwapBuffers(_handle);
}

void Window::cleanup() {
    if(_handle) {
        glfwDestroyWindow(_handle);
        _handle = nullptr; 
    }
    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}  

Window::Window() {
}

Window::~Window() {
    cleanup();
}