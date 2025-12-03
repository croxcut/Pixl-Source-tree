#ifndef __RENDERER__
#define __RENDERER__

#include "IRenderer.h"
#include "opengl/OpenGLRenderer.h"
#include "../../util/Util.h"

enum RendererAPI{
    OPENGL = 0x0,
    VULKAN,
    DIRECTX12
};

class Renderer : public IRenderer {

public:

    static IRenderer* getInstance(RendererAPI api) {
        static IRenderer* instance[] = {
            new OpenGLRenderer(),
            nullptr,
            nullptr
        };
        return instance[api];
    }

private:

    Renderer() = default;
    ~Renderer() = default;

};

#endif