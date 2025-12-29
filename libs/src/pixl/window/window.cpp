#include "pixl/window/window.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  

Window::Window(struct WindowOpts& opts) {
    LOG("Window Created w: %llu | h: %llu", opts.width, opts.height);

    if(!glfwInit()) ERROR("Failed to initialize GLFW!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(
        opts.width,
        opts.height,
        opts.title,
        nullptr,
        nullptr
    );

    if(!handle) {
        ERROR("Failed to create Window!"); 
        cleanup();
    } 

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG("Failed to load GLAD!");
        cleanup();
    }    

    glViewport(0, 0, opts.width, opts.height);
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);  

    glfwSwapInterval(0);    
}

Window::~Window() {
    cleanup();
}

Window* Window::create_window(struct WindowOpts& opts) {
    static Window _self(opts);
    return &_self;
}

bool Window::close() {
    return handle ? glfwWindowShouldClose(handle) : true;
}

void Window::refresh() {
    glfwSwapBuffers(handle);
}

void Window::poll_events() {
    glfwPollEvents();
}

void Window::cleanup() {
    if(handle) 
        glfwDestroyWindow(handle);

    glfwTerminate();
    LOG("Window Resources dumped");
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  