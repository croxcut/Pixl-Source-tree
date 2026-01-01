#include "pixl/window/window.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  

Window::Window() {
    // opts already initialized with default values
}

Window::~Window() {
    cleanup();
}

Window* Window::create_window() {
    static Window _self;

    if (!_self.handle) {
        _self.init(); 
    }

    return &_self;
}

void Window::set_opts(const WindowOpts& new_opts) {
    opts = new_opts;
}

void Window::init() {
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
        return;
    } 

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG("Failed to load GLAD!");
        cleanup();
        return;
    }    
   
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);  

    glfwSwapInterval(0); 
}

void Window::toggle_vsync() {
    vsync = !vsync;
    glfwSwapInterval(vsync);        
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
    if(handle) {
        glfwDestroyWindow(handle);
        handle = nullptr; // important to prevent re-destroy
    }

    glfwTerminate();
    LOG("Window Resources dumped");
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}  

bool Window::key_down(int key) {
    if (!handle) return false;
    return glfwGetKey(handle, key) == GLFW_PRESS;
} 