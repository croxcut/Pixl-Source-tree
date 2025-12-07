#include "pixl/engine/gfx/Window.h"

#include <glad/glad.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window(IAppLogic& logic) : appLogic(logic) {
    LOG(INFO, "Initializing Engine...");

    if (!glfwInit()) {
        LOG(ERROR, "Failed to Initialize GLFW!");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(fboWidth, fboHeight, "Pixl Engine Test", nullptr, nullptr);
    if (!handle) {
        LOG(ERROR, "Failed to create Window!");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(handle);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG(ERROR, "Failed to initialize GLAD!");
        return;
    }

    glViewport(0, 0, fboWidth, fboHeight);
    glfwSetFramebufferSizeCallback(handle, framebuffer_size_callback);
    glfwSwapInterval(0);

    glEnable(GL_DEPTH_TEST); 

    LOG(INFO, "Window created successfully!");
}

Window::~Window() {
    cleanup();
}

void Window::_init() {
    appLogic.init();

    imgui = new ImGuiManager(handle);
    imgui->init();

    glGenFramebuffers(1, &gameFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);

    glGenTextures(1, &gameColorTex);
    glBindTexture(GL_TEXTURE_2D, gameColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameColorTex, 0);

    glGenRenderbuffers(1, &gameDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gameDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameDepthRBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG(ERROR, "Game FBO incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::_tick() {
    appLogic.tick();
    imgui->startFrame();

    setupDocking(); 

    ImGui::Begin("Game");
    ImVec2 gamePanelSize = ImGui::GetContentRegionAvail();
    int newWidth = (int)gamePanelSize.x;
    int newHeight = (int)gamePanelSize.y;
    if (newWidth != fboWidth || newHeight != fboHeight)
        resizeFBO(newWidth, newHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);
    glViewport(0, 0, fboWidth, fboHeight);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    appLogic.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, fboWidth, fboHeight);

    ImGui::Image((void*)(intptr_t)gameColorTex, gamePanelSize, ImVec2(0,1), ImVec2(1,0));
    ImGui::End();

    ImGui::Begin("Properties"); 
    ImGui::End();

    ImGui::Begin("Scene"); 
    ImGui::End();
}

void Window::_render() {
    glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);
    glViewport(0, 0, fboWidth, fboHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    appLogic.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, fboWidth, fboHeight);

    if (imgui) imgui->endFrame();
}

GLFWwindow* Window::getWindow() {
    return handle;
}

Window& Window::createWindow(IAppLogic& logic) {
    static Window window(logic);
    return window;
}

void Window::start() {
    _init();

    while (!glfwWindowShouldClose(handle)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _tick();
        _render();

        glfwSwapBuffers(handle);
        glfwPollEvents();
    }

    cleanup();
}

void Window::cleanup() {
    if (imgui) {
        imgui->cleanup();
        delete imgui;
        imgui = nullptr;
    }

    if(gameFBO) {
        glDeleteFramebuffers(1, &gameFBO);
        glDeleteTextures(1, &gameColorTex);
        glDeleteRenderbuffers(1, &gameDepthRBO);
    }

    appLogic.cleanup();
    glfwTerminate();
}
