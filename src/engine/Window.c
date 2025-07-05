#include "Window.h"

struct Window window;

static void _size_callback(GLFWwindow* window, int width, int height);  

void window_create(FWindow init, FWindow render, FWindow update, FWindow cleanup) {
    window.init = init;
    window.update = update;
    window.render = render;
    window.cleanup = cleanup;
    
    printf("[Engine] : Starting...\n");

    if(!glfwInit()) {
        printf("[Engine] : Failed to Initialize GLFW!\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window.handle = glfwCreateWindow(600, 400, "Just A Window", NULL, NULL);
    if(window.handle == NULL) {
        printf("[Engine] : Failed to Create Window!\n");
        return;    
    }
    
    glfwMakeContextCurrent(window.handle);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("[Engine] : Failed to initialize Glad!\n");
        glfwTerminate();
        return;
    }

    glViewport(0, 0, 600, 400);

    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window.handle, _size_callback);
}   


static void _init() {
    printf("[Engine] : Intializing...\n");
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

    glfwTerminate();
    printf("[Engine] : Shutdown...");
}

void loop() {
    _init();

    printf("[Engine] : Running...");

    while(!glfwWindowShouldClose(window.handle)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        _update();

        _render();

        glfwSwapBuffers(window.handle);
        glfwPollEvents();    
    }
    
    _cleanup();
}

static void _size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}  