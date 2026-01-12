#ifndef CONFIG_H
#define CONFIG_H

#define PIXL_LOGGER_ENABLED

#define PIXL_RENDERER_BACKEND_OPENGL
#define PIXL_RENDERER_BACKEND_VULKAN
#define PIXL_RENDERER_BACKEND_DX3D12

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define ENGINE_TITLE \
    "Pixl Engine v" STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH)

struct WindowConfig {
    int width = 1280;
    int height = 720;
    const char* title = ENGINE_TITLE;
};

#endif
