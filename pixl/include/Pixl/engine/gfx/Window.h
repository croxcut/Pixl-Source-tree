#ifndef __WINDOW__
#define __WINDOW__

#include "../core/IAppLogic.h"
#include "../util/Log.h"
#include "../util/Util.h"
#include "../ui/ImGuiManager.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct EngineTiming {
    bool vsync = false;
    bool capFPS = false;
    int maxFPS = 144;

    bool fixedTick = true;
    double tickRate = 60.0; 
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

    EngineTiming timing;

    double lastTime = 0.0;
    float fps = 0.0f;
    float tps = 0.0f;
    int ticksThisFrame = 0;

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
