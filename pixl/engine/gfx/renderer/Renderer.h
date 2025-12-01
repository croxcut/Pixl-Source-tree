#ifndef __RENDERER__
#define __RENDERER__

#include "IGraphics.h"
#include "GraphicsApi.h"
#include "../../util/Util.h"

class Renderer {

public:
    static Renderer& get();

    void init(GraphicsApi api);

    IGraphics& device();

    void draw();
    void cleanup();

private:

    Renderer() = default;
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator = (const Renderer&) = delete;

    IGraphics* m_device = nullptr;

};

#endif