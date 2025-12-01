
#include "../../pixl/engine/Window.h"
#include "../../pixl/engine/gfx/renderer/opengl/shader/GLShader.h"
#include "../../pixl/engine/gfx/renderer/opengl/OpenGLRenderer.h"

#define STB_IMAGE_IMPLEMENTATION

class Zvezda : public IAppLogic{

private:

    GLShader *shader = nullptr;
    IRenderer *renderer = nullptr;
    GLMesh *mesh = nullptr;

public:

    void init() override {

        shader = new GLShader("res/shader/vertex.glsl", "res/shader/fragment.glsl");

        mesh = new GLMesh();
        mesh->vertices = {
            { 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f},
            { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
            {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
            {-0.5f,  0.5f, 0.0f, 5.0f, 0.5f, 0.0f}, 
        };
        mesh->indices = {
            0, 1, 3,  
            1, 2, 3   
        };

        renderer = new OpenGLRenderer(shader, mesh);
        renderer->init();

    };

    void tick() override {
        renderer->update();
    };

    void render() override {
        renderer->draw();
    };

    void cleanup() override {
        delete shader;
        delete mesh;
        renderer->cleanup();
    }

};

int main(int argc, char* argv[]) {

    Zvezda zvezda;
    Window window = Window::createWindow(zvezda);
    window.start();

    return 0;
}


