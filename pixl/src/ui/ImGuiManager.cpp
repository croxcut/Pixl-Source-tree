#include "pixl/engine/ui/ImGuiManager.h"

void ImGuiManager::init() {
    LOG(INFO, "GUI MANAGER INITIALIZING!...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Make sure window handle is valid
    if (!_windowHandle) {
        LOG(ERROR, "ImGuiManager init failed: invalid GLFW window handle!");
        return;
    }

    ImGui_ImplGlfw_InitForOpenGL(_windowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    LOG(INFO, "GUI MANAGER INITIALIZED!");
}

void ImGuiManager::startFrame() {
    // Ensure context is current
    if (!_windowHandle) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::endFrame() {
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

void ImGuiManager::cleanup() {
    LOG(INFO, "GUI MANAGER RESOURCE CLEANUP!...");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    LOG(INFO, "GUI MANAGER RESOURCES FREED!");
}

void ImGuiManager::tick() {
    
}

void ImGuiManager::draw() {
    
}

