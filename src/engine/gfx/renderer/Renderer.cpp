#include "Renderer.h"

#include "opengl/OpenGLDevice.h"

Renderer& Renderer::get() {
    static Renderer instance;
    return instance;
}

void Renderer::init(GraphicsApi api) {
    std::cout << "Renderer : Started" << std::endl;

    switch(api) {
        case GraphicsApi::Opengl:
            m_device = &OpenGLDevice::get();
            break;
        case GraphicsApi::Vulkan:
            break;
        case GraphicsApi::DirectX12:
            break;
    }
}

void Renderer::draw() {
    std::cout << "Renderer : Draw" << std::endl;

    if(m_device) m_device->draw();
}

IGraphics& Renderer::device() {
    return *m_device;
}

void Renderer::cleanup() {
    if(m_device) m_device->cleanup();
}