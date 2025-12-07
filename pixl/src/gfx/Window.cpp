#include "pixl/engine/gfx/Window.h"
#include <iostream>

// forward: framebuffer callback
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
    glfwSwapInterval(0);

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

void Window::setupDocking()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Fullscreen dockspace window
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("DockspaceWindow", nullptr, window_flags);

    // Dockspace
    ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
    if (!ImGui::DockBuilderGetNode(dockspace_id)) {
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_left = dockspace_id;
        ImGuiID dock_right = 0;
        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.35f, &dock_right, &dock_left);

        ImGuiID dock_scene = 0;
        ImGuiID dock_props = 0;
        ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Up, 0.5f, &dock_scene, &dock_props);

        ImGui::DockBuilderDockWindow("Game", dock_left);
        ImGui::DockBuilderDockWindow("Scene", dock_scene);
        ImGui::DockBuilderDockWindow("Properties", dock_props);

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
}


void Window::_tick()
{
    appLogic.tick();
    imgui->startFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("Scene", nullptr, &showSceneWindow);
            ImGui::MenuItem("Properties", nullptr, &showPropertiesWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

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

    ImGui::Image((void*)(intptr_t)gameColorTex, gamePanelSize, ImVec2(0,1), ImVec2(1,0));
    ImGui::End();

    if (showSceneWindow) {
        ImGui::Begin("Scene", &showSceneWindow);

        auto& objs = appLogic.getSceneObjects();
        int& selectedIndex = appLogic.getSelectedObjectIndex();

        if (objs.empty()) {
            ImGui::Text("No objects in scene");
        } else {
            ImGui::Text("Scene Objects:");
            ImGui::Separator();
            ImGui::BeginChild("SceneList", ImVec2(0,0), false);
            for (int i = 0; i < (int)objs.size(); ++i) {
                bool isSelected = (selectedIndex == i);
                if (ImGui::Selectable(objs[i].meshId.c_str(), isSelected))
                    selectedIndex = i;
            }
            ImGui::EndChild();
        }

        ImGui::Separator();
        if (ImGui::Button("Create Empty Object")) {
            std::string id = "object_" + std::to_string(objs.size());
            SceneObject obj; obj.meshId = id;
            objs.push_back(obj);
            selectedIndex = (int)objs.size() - 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Selected") && !objs.empty()) {
            if (selectedIndex >= 0 && selectedIndex < (int)objs.size()) {
                objs.erase(objs.begin() + selectedIndex);
                selectedIndex = (int)objs.size() - 1;
                if (selectedIndex < 0) selectedIndex = -1;
            }
        }

        ImGui::End();
    }

    if (showPropertiesWindow) {
        ImGui::Begin("Properties", &showPropertiesWindow);

        auto& objs = appLogic.getSceneObjects();
        int& selectedIndex = appLogic.getSelectedObjectIndex();

        if (selectedIndex < 0 || selectedIndex >= (int)objs.size()) {
            ImGui::Text("No object selected");
        } else {
            auto& obj = objs[selectedIndex];
            ImGui::Text("Selected: %s", obj.meshId.c_str());
            ImGui::Separator();
            ImGui::DragFloat3("Position", glm::value_ptr(obj.position), 0.05f);
            ImGui::DragFloat3("Rotation (deg)", glm::value_ptr(obj.rotation), 1.0f);
            ImGui::DragFloat3("Scale", glm::value_ptr(obj.scale), 0.05f, 0.001f, 100.0f);
            ImGui::Separator();
            if (ImGui::Button("Reset Transform")) {
                obj.position = glm::vec3(0.0f);
                obj.rotation = glm::vec3(0.0f);
                obj.scale = glm::vec3(1.0f);
            }
        }

        ImGui::End();
    }
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
