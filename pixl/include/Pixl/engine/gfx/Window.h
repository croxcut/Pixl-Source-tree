#ifndef __WINDOW__
#define __WINDOW__

#include "../core/IAppLogic.h"
#include "../util/Log.h"
#include "../util/Util.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h> // DockBuilder

class ImGuiManager {
private:
    GLFWwindow* window;
public:
    ImGuiManager(GLFWwindow* win) : window(win) {}

    void init() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void startFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void endFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_context);
        }
    }

    void cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

class Window {
private:
    GLFWwindow* handle;
    IAppLogic& appLogic;
    ImGuiManager* imgui = nullptr;

    GLuint gameFBO = 0;
    GLuint gameColorTex = 0;
    GLuint gameDepthRBO = 0;
    int fboWidth = 1280;
    int fboHeight = 720;
    
    void initFBO() {
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

    void resizeFBO(int width, int height) {
        fboWidth = width;
        fboHeight = height;

        glBindTexture(GL_TEXTURE_2D, gameColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glBindRenderbuffer(GL_RENDERBUFFER, gameDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void setupDocking() {
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            // Split the dockspace: main (left 80%) and right panel (20%)
            ImGuiID dock_id_right = 0;
            ImGuiID dock_id_main = dockspace_id;
            ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.20f, &dock_id_right, &dock_id_main);

            // Split the right panel vertically: top 50% Properties, bottom 50% Scene
            ImGuiID dock_id_right_top = 0;
            ImGuiID dock_id_right_bottom = 0;
            ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.50f, &dock_id_right_top, &dock_id_right_bottom);

            // Dock windows
            ImGui::DockBuilderDockWindow("Game", dock_id_main);          // Left/main viewport
            ImGui::DockBuilderDockWindow("Properties", dock_id_right_top); // Right-top panel
            ImGui::DockBuilderDockWindow("Scene", dock_id_right_bottom);   // Right-bottom panel

            ImGui::DockBuilderFinish(dockspace_id);
        }

        ImGui::DockSpaceOverViewport(dockspace_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void _init();
    void _tick();
    void _render();

public:
    Window(IAppLogic& logic);
    ~Window();

    static Window& createWindow(IAppLogic& logic);
    GLFWwindow* getWindow();
    void start();
    void cleanup();
};

#endif
