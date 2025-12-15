#ifndef __IMGUI_MANAGER__
#define __IMGUI_MANAGER__

#include <GLFW/glfw3.h>

#include "../core/Flow.h"
#include "../util/Log.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <filesystem>
#include <vector>
#include <string>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

namespace fs = std::filesystem;

inline std::string getExeFolder() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return fs::canonical(buffer).parent_path().string();
#elif __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) return fs::canonical(std::string(result, count)).parent_path().string();
    return fs::current_path().string();
#elif __APPLE__
    char buffer[1024];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0)
        return fs::canonical(buffer).parent_path().string();
    return fs::current_path().string();
#else
    return fs::current_path().string();
#endif
}

class ImGuiManager : Flow{

// ---- Variables ----
public:
    GLFWwindow* _windowHandle = nullptr;    

private:

// ---- Functions ----
public:
    ImGuiManager(GLFWwindow* windowHandle) : _windowHandle(windowHandle) {}

    void init() override;
    void tick() override;
    void draw() override;
    void cleanup() override;

    void startFrame(); 
    void endFrame();

private:

    ImGuiManager() = default;
    
};

#endif