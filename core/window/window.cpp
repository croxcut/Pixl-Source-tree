/*==============================================================================
    PIXL ENGINE

    Copyright (C) 2026 John Paul Valenzuela
    Author : John Paul Valenzuela
    Email  : johnpaulvalenzuelaog@gmail.com

    This file is part of PIXL ENGINE.

    PIXL ENGINE is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PIXL ENGINE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PIXL ENGINE.  If not, see <https://www.gnu.org/licenses/>.
==============================================================================*/

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