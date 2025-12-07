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
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    // Offscreen FBO that renders the game scene (displayed inside ImGui Game panel)
    GLuint gameFBO = 0;
    GLuint gameColorTex = 0;
    GLuint gameDepthRBO = 0;
    int fboWidth = 1280;
    int fboHeight = 720;

    // Editor window visibility (toggled from menu)
    bool showSceneWindow = true;
    bool showPropertiesWindow = true;

    // internals
    void initFBO();
    void resizeFBO(int width, int height);
    void setupDocking();

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
