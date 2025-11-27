#include "Window.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

Window::Window(IAppLogic& logic) : 
    appLogic(logic) 
{

    std::cout << "[Engine] : Started" << std::endl;

    if(!glfwInit()) {
        std::cout << "Failed to Initialize GLFW!" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(800, 600, "TestWindow", NULL, NULL);
    if(handle == NULL) {
        std::cout << "Failed to create Window" << std::endl;
        glfwTerminate();
        return;
    } 

    glfwMakeContextCurrent(handle);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }    

    glViewport(0, 0, 800, 600);
    
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);  

    glfwSwapInterval(0);
}

Window::~Window() {
    cleanup();
}

void Window::cleanup() {
    appLogic.cleanup();
    glfwTerminate();
}

GLFWwindow* Window::getWindow() {
    return handle;
}

Window& Window::createWindow(IAppLogic& logic) {
    static Window window(logic);
    return window;
}

void Window::_init() {
    appLogic.init();
}

void Window::_tick() {
    appLogic.tick();
}

void Window::_render() {
    appLogic.render();
}

void Window::start() {

    _init();

    while(!glfwWindowShouldClose(handle)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        _tick();
        _render();

        glfwSwapBuffers(handle);
        glfwPollEvents();
    }
    
    // cleanup();
}