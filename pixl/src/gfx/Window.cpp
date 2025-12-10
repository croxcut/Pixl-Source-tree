#include "pixl/engine/gfx/Window.h"
#include <iostream>
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
}

void Window::_tick() {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    appLogic.tick();
    imgui->startFrame();

    setupDocking(); 

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

    ImGui::Image((void*)(intptr_t)gameColorTex, gamePanelSize, ImVec2(0,1), ImVec2(1,0));

    if (showFPSViewer) {
        char fpsText[32];
        snprintf(fpsText, sizeof(fpsText), "FPS: %.1f", 1.0f / deltaTime);
        ImVec2 textSize = ImGui::CalcTextSize(fpsText);
        ImGui::SetCursorPos(ImVec2(gamePanelSize.x - textSize.x,
                                    gamePanelSize.y - textSize.y));
        ImGui::Text("%s", fpsText);
    }

    ImGui::End();
    ImGui::PopStyleVar();

    if (showSceneWindow) {
        ImGui::Begin("Scene", &showSceneWindow);
        auto& objs = appLogic.getSceneObjects();
        int& selectedIndex = appLogic.getSelectedObjectIndex();

        if (objs.empty()) ImGui::Text("No objects in scene");
        else {
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

        ImGui::End();
    }

    if (showPropertiesWindow) {
        ImGui::Begin("Properties", &showPropertiesWindow);
        auto& objs = appLogic.getSceneObjects();
        int& selectedIndex = appLogic.getSelectedObjectIndex();

        if (selectedIndex >= 0 && selectedIndex < (int)objs.size()) {
            auto& obj = objs[selectedIndex];
            ImGui::Text("Selected: %s", obj.meshId.c_str());

            auto float3WithSingleIncDec = [](const char* label, glm::vec3& value, float step = 0.05f) {
                ImGui::Text("%s", label);
                ImGui::SameLine(100);

                const char* axes[3] = { "X", "Y", "Z" };
                for (int i = 0; i < 3; ++i) {
                    ImGui::PushID(label);
                    ImGui::PushID(i);

                    ImGui::BeginGroup();

                    ImGui::PushItemWidth(60);
                    ImGui::InputFloat("##value", &value[i], 0.0f, 0.0f, "%.3f");
                    ImGui::PopItemWidth();

                    ImVec2 btnSize(18.0f, ImGui::GetItemRectSize().y);
                    ImGui::SameLine(0, 0);
                    if (ImGui::InvisibleButton("##incdec", btnSize)) {
                        ImVec2 mousePos = ImGui::GetMousePos();
                        ImVec2 btnMin = ImGui::GetItemRectMin();
                        float halfY = btnSize.y * 0.5f;
                        if (mousePos.y < btnMin.y + halfY) value[i] += step;
                        else value[i] -= step;
                    }

                    ImDrawList* draw = ImGui::GetWindowDrawList();
                    ImVec2 btnMin = ImGui::GetItemRectMin();
                    ImVec2 btnMax = ImGui::GetItemRectMax();

                    float centerX = (btnMin.x + btnMax.x) * 0.5f;
                    float btnHeight = btnMax.y - btnMin.y;
                    float quarterH = btnHeight * 0.25f;
                    float s = 3.0f; 

                    draw->AddTriangleFilled(
                        ImVec2(centerX - s, btnMin.y + quarterH + s),
                        ImVec2(centerX + s, btnMin.y + quarterH + s),
                        ImVec2(centerX,     btnMin.y + quarterH - s),
                        IM_COL32(255, 255, 255, 255)
                    );

                    draw->AddTriangleFilled(
                        ImVec2(centerX - s, btnMin.y + 3 * quarterH - s),
                        ImVec2(centerX + s, btnMin.y + 3 * quarterH - s),
                        ImVec2(centerX,     btnMin.y + 3 * quarterH + s),
                        IM_COL32(255, 255, 255, 255)
                    );

                    ImGui::EndGroup();

                    if (i < 2) ImGui::SameLine(0, 10);

                    ImGui::PopID();
                    ImGui::PopID();
                }
            };

            float3WithSingleIncDec("Position", obj.position, 0.05f);
            float3WithSingleIncDec("Rotation", obj.rotation, 1.0f);
            float3WithSingleIncDec("Scale", obj.scale, 0.05f);

        } else {
            ImGui::Text("No object selected");
        }

        ImGui::End();
    }

    if (showFoldersWindow) {
        ImGui::Begin("Folders", &showFoldersWindow);
        ImGui::Text("Path: %s", currentFolder.c_str());
        ImGui::Separator();
        if (ImGui::Button("..")) {
            fs::path p(currentFolder);
            if (p.parent_path().string() != exeFolder) {
                currentFolder = p.parent_path().string();
                refreshFolders();
            }
        }
        ImGui::BeginChild("FolderList", ImVec2(0,0), true);
        for (auto& entry : folderEntries) {
            std::string label = "[DIR] " + entry.path().filename().string();
            if (ImGui::Selectable(label.c_str())) {
                currentFolder = entry.path().string();
                refreshFolders();
            }
        }
        ImGui::EndChild();
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

GLFWwindow* Window::getWindow() { return handle; }

Window& Window::createWindow(IAppLogic& logic) {
    static Window window(logic);
    return window;
}

void Window::start() {
    _init();
    while (!glfwWindowShouldClose(handle)) {
        glClearColor(0.1f,0.1f,0.1f,1.0f);
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
