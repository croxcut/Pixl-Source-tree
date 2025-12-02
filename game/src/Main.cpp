#include "../../pixl/engine/Window.h"
#include "../../pixl/engine/gfx/renderer/opengl/shader/GLShader.h"
#include "../../pixl/engine/gfx/renderer/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION

class Zvezda : public IAppLogic {

private:
    IRenderer* renderer = nullptr;
    
    std::string squareMeshId;
    std::string triangleMeshId;
    std::string colorShaderId;
    std::string gradientShaderId;

public:

    void init() override {
        renderer = Renderer::getInstance(OPENGL);

        Mesh squareMesh = {
            {
                { 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f},  
                { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},  
                {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},  
                {-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 1.0f},  
            },
            {
                0, 1, 3,  
                1, 2, 3   
            }
        };
        squareMeshId = renderer->createMesh(squareMesh);

        Mesh triangleMesh = {
            {
                { 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f},  
                { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},  
                {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f},  
            },
            {
                0, 1, 2  
            }
        };
        triangleMeshId = renderer->createMesh(triangleMesh);

        Shader colorShader = {
            "res/shader/vertex.glsl", 
            "res/shader/fragment.glsl"
        };
        colorShaderId = renderer->createShader(colorShader);

        Shader gradientShader = {
            "res/shader/vertex.glsl", 
            "res/shader/fragment.glsl"  
        };
        gradientShaderId = renderer->createShader(gradientShader);
        
        renderer->init();

    };  

    void tick() override {
        renderer->update();
    };

    void render() override {
        renderer->useShader(colorShaderId);

        renderer->drawMesh(squareMeshId);
        
        renderer->useShader(gradientShaderId);

        renderer->drawMesh(triangleMeshId);
        
    };

    void cleanup() override {
        if (renderer) {
            renderer->cleanup();
            delete renderer;
            renderer = nullptr;
        }
    }

};

int main(int argc, char* argv[]) {
    Zvezda zvezda;
    Window window = Window::createWindow(zvezda);
    window.start();
    return 0;
}