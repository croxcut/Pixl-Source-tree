
#include "../../pixl/engine/Window.h"
#include "../../pixl/engine/gfx/renderer/opengl/shader/GLShader.h"
#include "../../pixl/engine/gfx/renderer/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION

class Zvezda : public IAppLogic{

private:

    GLShader *shader = nullptr;
    IRenderer *renderer = nullptr;

public:

    void init() override {

        renderer = Renderer::getInstance(OPENGL);

        Mesh mesh = {
          {
            { 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f},
            { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
            {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
            {-0.5f,  0.5f, 0.0f, 5.0f, 0.5f, 0.0f}, 
          } ,
          {
            0, 1, 3,  
            1, 2, 3   
          } 
        };
        renderer->createMesh(mesh);

        Shader shader = {
            "res/shader/vertex.glsl", 
            "res/shader/fragment.glsl"
        };
        renderer->createShader(shader);
        
        renderer->init();
    };  

    void tick() override {
        renderer->update();
    };

    void render() override {
        renderer->draw();
    };

    void cleanup() override {
        renderer->cleanup();
        shader->cleanup();
        delete renderer;
        delete shader;
    }

};

int main(int argc, char* argv[]) {

    Zvezda zvezda;
    Window window = Window::createWindow(zvezda);
    window.start();

    return 0;
}


