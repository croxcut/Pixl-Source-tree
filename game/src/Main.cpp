#include <iostream>

#include <pixl/engine/engine.h>
#include <pixl/engine/renderer/backend/opengl.h>
#include <pixl/engine/renderer/renderer.h>

class App : public IAppLogic {
private:
    Renderer* renderer = nullptr;           
    u64 mesh_id = 0;
    u64 shader_id = 0;
    OpenGL opengl;
public:
    void init() override {

        renderer = &opengl;

        Mesh triangle {
            {
                { -0.5f, -0.5f, 0.0f,  1, 0, 0,    0, 0 },
                {  0.5f, -0.5f, 0.0f,  0, 1, 0,    1, 0 },
                {  0.0f,  0.5f, 0.0f,  0, 0, 1,  0.5, 1 }
            },
            {
                { 0, 1, 2 }
            }
        };

        mesh_id = renderer->add_mesh(triangle);

        Shader shader{
            "res/shaders/triangle.vert",
            "res/shaders/triangle.frag"
        };

        shader_id = renderer->add_shader(shader);
    }

    void tick(const f32& dt) override {

    }

    void render() override {
        DrawCall call{};
        call.mesh_id = mesh_id;
        call.shader_id = shader_id;

        renderer->submit_draw_call(call);
        renderer->draw();
    }

    void cleanup() override {
        renderer->cleanup();
    }
};

int main() {
    App app;
    Engine engine(app);
    engine.start();
    return 0;
}