
#include <pixl/engine/gfx/Window.h>
#include <pixl/engine/gfx/renderer/Renderer.h>

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
                { 0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,   1.0f, 1.0f},  
                { 0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f},  
                {-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f},  
                {-0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 1.0f,   0.0f, 1.0f},  
            },
            {
                0, 1, 3,  
                1, 2, 3   
            }, 
            {
                "res/texture/0x1.jpg"
            }
        };
        squareMeshId = renderer->createMesh(squareMesh);

        Shader colorShader = {
            "res/shader/vertex.glsl", 
            "res/shader/fragment.glsl"
        };
        colorShaderId = renderer->createShader(colorShader);
        
        renderer->init();

    };  

    void tick() override {
        renderer->tick();
    };

    void draw() override {
        renderer->useShader(colorShaderId);
        renderer->drawMesh(squareMeshId);

        // renderer->draw();
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