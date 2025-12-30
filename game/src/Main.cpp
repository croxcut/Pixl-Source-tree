#include <iostream>

#include <pixl/window/window.h>
#include <pixl/engine/engine.h>
#include <pixl/engine/renderer/backend/gl_mesh.h>
#include <pixl/engine/renderer/backend/gl_shader.h>
#include <pixl/engine/renderer/backend/opengl.h>
#include <pixl/engine/renderer/renderer.h>

class App : public IAppLogic {
private:
    GLMesh mesh;
    GLShader shader;

    Renderer* renderer = nullptr;

public:
    void init() {
        OpenGL opengl;
        renderer = &opengl;

        Mesh triangle {
            {
                {-0.5f, -0.5f, 0.0f },
                { 0.5f, -0.5f, 0.0f }, 
                { 0.0f,  0.5f, 0.0f } 
            }
        };

        mesh.create_mesh(triangle);

        Shader source{
            "res/shaders/triangle.vert",
            "res/shaders/triangle.frag"
        };

        shader.create_shader(source);
    }

    void tick(const f32& dt) {

    }

    void render() {
        shader.use();
        mesh.draw();
        shader.clear();
    }

    void cleanup() {

    }

};

int main(int argc, char* argv[]) {

    App app;
    Engine engine(app);
    engine.start();
    
    return 0;
}