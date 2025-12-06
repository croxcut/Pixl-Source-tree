
#include <pixl/engine/gfx/Window.h>
#include <pixl/engine/gfx/renderer/Renderer.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Zvezda : public IAppLogic {

private:

    Assimp::Importer importer;

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

        float scale = 1.2f;

        float time = glfwGetTime();
        float rotation = time * 5.0f;

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
        transform = glm::scale(transform, glm::vec3(scale, scale, scale));

        renderer->submitDrawCall(squareMeshId, colorShaderId, transform);

        renderer->draw();  
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