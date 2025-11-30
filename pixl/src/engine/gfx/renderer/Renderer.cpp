#include "Renderer.h"

#include "opengl/OpenGLDevice.h"

Renderer& Renderer::get() {
    static Renderer instance;
    return instance;
}

void Renderer::init(GraphicsApi api) {
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
    if(m_device) m_device->draw();
}

IGraphics& Renderer::device() {
    assert(m_device != nullptr);
    return *m_device;
}

void Renderer::cleanup() {
    if(m_device) m_device->cleanup();
}