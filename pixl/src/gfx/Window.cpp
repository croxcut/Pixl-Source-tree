#include "pixl/engine/gfx/Window.h"
#include <iostream>
#include <thread>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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

    glfwSwapInterval(timing.vsync ? 1 : 0);

    glEnable(GL_DEPTH_TEST);
    LOG(INFO, "Window created successfully!");
}

Window::~Window() {
    cleanup();
}

void Window::initFBO() {
    glGenTextures(1, &gameColorTex);
    glBindTexture(GL_TEXTURE_2D, gameColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &gameDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gameDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);

    glGenFramebuffers(1, &gameFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameColorTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameDepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG(ERROR, "Game FBO not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::resizeFBO(int width, int height) {
    if (width <= 0 || height <= 0) return;

    fboWidth = width;
    fboHeight = height;

    glBindTexture(GL_TEXTURE_2D, gameColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, gameDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Window::refreshFolders() {
    folderEntries.clear();
    try {
        for (auto& entry : fs::directory_iterator(currentFolder)) {
            if (entry.is_directory())
                folderEntries.push_back(entry);
        }
    } catch (...) {
        LOG(ERROR, "Failed to read folder: %s", currentFolder.c_str());
    }
}

void Window::setupDocking() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_MenuBar; 

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("DockspaceWindow", nullptr, window_flags);

    if (ImGui::BeginMenuBar()) {
        // --------------------
        // Main Menu Items
        // --------------------
        if (ImGui::BeginMenu("Widgets")) {
            ImGui::MenuItem("Scene", nullptr, &showSceneWindow);
            ImGui::MenuItem("Properties", nullptr, &showPropertiesWindow);
            ImGui::MenuItem("Folders", nullptr, &showFoldersWindow);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Info")) {
            ImGui::MenuItem("FPS Viewer", nullptr, &showFPSViewer);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Performance")) {
            ImGui::Checkbox("VSync", &timing.vsync);

            // Replace sliders with input fields
            ImGui::InputInt("Max FPS", &timing.maxFPS);
            ImGui::InputFloat("Tick Rate", (float*)&timing.tickRate);
            ImGui::Checkbox("Cap FPS", &timing.capFPS);
            ImGui::Checkbox("Fixed Tick", &timing.fixedTick);
            ImGui::EndMenu();
        }
        {
            ImGuiStyle& style = ImGui::GetStyle();
            float windowWidth = ImGui::GetWindowWidth();
            float textWidth = ImGui::CalcTextSize("FPS: 000 | TPS: 000").x; // approximate width
            ImGui::SameLine(windowWidth - textWidth - style.FramePadding.x * 2);
            ImGui::Text("FPS: %d | TPS: %d", (int)fps, (int)tps + 1);            
        }

        ImGui::EndMenuBar();
    }

    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    if (!ImGui::DockBuilderGetNode(dockspace_id)) {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_right = 0;
        ImGuiID dock_bottom = 0;

        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.35f, &dock_right, &dock_main);
        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.25f, &dock_bottom, &dock_main);

        ImGuiID dock_scene = 0;
        ImGuiID dock_props = 0;

        ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Up, 0.5f, &dock_scene, &dock_props);

        ImGui::DockBuilderDockWindow("Game", dock_main);
        ImGui::DockBuilderDockWindow("Scene", dock_scene);
        ImGui::DockBuilderDockWindow("Properties", dock_props);
        ImGui::DockBuilderDockWindow("Folders", dock_bottom);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0,0), ImGuiDockNodeFlags_None);
    ImGui::End();
    ImGui::PopStyleVar(3);
}

void Window::_init() {
    appLogic.init();
    imgui = new ImGuiManager(handle);
    imgui->init();
    initFBO();
    refreshFolders();
    lastFPSTime = glfwGetTime();
    frameCount = 0;
    tickCount = 0;
    fps = 0.0f;
    tps = 0.0f;
}

void Window::_tick() {
    static bool lastVsync = timing.vsync;
    if (lastVsync != timing.vsync) {
        glfwSwapInterval(timing.vsync ? 1 : 0);
        lastVsync = timing.vsync;
    }

    appLogic.tick(); // FIXED RATE GAME LOGIC
    tickCount++;
}

void Window::_render() {
    imgui->startFrame();

    setupDocking();

    // --------------------
    // GAME WINDOW
    // --------------------
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
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

    ImGui::Image(
        (void*)(intptr_t)gameColorTex,
        gamePanelSize,
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::End();
    ImGui::PopStyleVar();

    // --------------------
    // OPTIONAL WINDOWS
    // --------------------
    if (showSceneWindow) {
        ImGui::Begin("Scene", &showSceneWindow);
        ImGui::End();
    }

    if (showPropertiesWindow) {
        ImGui::Begin("Properties", &showPropertiesWindow);
        ImGui::End();
    }

    if (showFoldersWindow) {
        ImGui::Begin("Folders", &showFoldersWindow);
        ImGui::Text("Path: %s", currentFolder.c_str());
        ImGui::Separator();
        ImGui::End();
    }

    imgui->endFrame();

    // --------------------
    // Update FPS/TPS
    // --------------------
    frameCount++;
    double currentTime = glfwGetTime();
    if (currentTime - lastFPSTime >= 1.0) {
        fps = frameCount / (currentTime - lastFPSTime);
        tps = tickCount / (currentTime - lastFPSTime);
        frameCount = 0;
        tickCount = 0;
        lastFPSTime = currentTime;
    }
}

GLFWwindow* Window::getWindow() { return handle; }

Window& Window::createWindow(IAppLogic& logic) {
    static Window window(logic);
    return window;
}

void Window::start() {
    _init();

    lastTime = glfwGetTime();
    float tickAccumulator = 0.0;

    const double tickStep = 1.0 / timing.tickRate;

    while (!glfwWindowShouldClose(handle)) {
        double now = glfwGetTime();
        double frameTime = now - lastTime;
        lastTime = now;

        // Avoid spiral of death
        if (frameTime > 0.25)
            frameTime = 0.25;

        tickAccumulator += frameTime;

        // --------------------
        // FIXED TICK (LOGIC)
        // --------------------
        while (timing.fixedTick && tickAccumulator >= tickStep) {
            _tick(); // FIXED RATE UPDATE
            tickAccumulator -= tickStep;
        }

        // --------------------
        // RENDER
        // --------------------
        _render();

        glfwSwapBuffers(handle);
        glfwPollEvents();

        // --------------------
        // FPS CAP (SOFTWARE)
        // --------------------
        if (timing.capFPS && !timing.vsync) {
            double targetFrame = 1.0 / timing.maxFPS;
            double endTime = glfwGetTime();
            double sleepTime = targetFrame - (endTime - now);

            if (sleepTime > 0.0) {
                std::this_thread::sleep_for(
                    std::chrono::duration<double>(sleepTime)
                );
            }
        }
    }

    cleanup();
}

void Window::cleanup() {
    if (imgui) {
        imgui->cleanup();
        delete imgui;
        imgui = nullptr;
    }

    if (gameFBO) {
        glDeleteFramebuffers(1, &gameFBO);
        glDeleteTextures(1, &gameColorTex);
        glDeleteRenderbuffers(1, &gameDepthRBO);
        gameFBO = 0;
        gameColorTex = 0;
        gameDepthRBO = 0;
    }

    appLogic.cleanup();
    glfwTerminate();
}
