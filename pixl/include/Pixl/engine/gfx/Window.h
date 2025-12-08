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

class ImGuiManager {
private:
    GLFWwindow* window;
public:
    ImGuiManager(GLFWwindow* win) : window(win) {}

    void init();
    void startFrame();
    void endFrame();
    void cleanup();
};

class Window {
private:
    GLFWwindow* handle;
    IAppLogic& appLogic;
    ImGuiManager* imgui = nullptr;

    bool showFPSViewer = false;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    GLuint gameFBO = 0;
    GLuint gameColorTex = 0;
    GLuint gameDepthRBO = 0;
    int fboWidth = 1280;
    int fboHeight = 720;

    bool showSceneWindow = true;
    bool showPropertiesWindow = true;
    bool showFoldersWindow = true;

    std::string exeFolder = getExeFolder();
    std::string currentFolder = exeFolder;
    std::vector<fs::directory_entry> folderEntries;

    void initFBO();
    void resizeFBO(int width, int height);
    void setupDocking();
    void refreshFolders();

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
