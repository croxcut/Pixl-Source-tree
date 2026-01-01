#include <iostream>

#include <pixl/engine/engine.h>
#include <pixl/engine/renderer/backend/opengl.h>
#include <pixl/engine/renderer/renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class App : public IAppLogic {
private:
    Renderer* renderer = nullptr;           
    u64 mesh_id = 0;
    u64 shader_id = 0;
    OpenGL opengl;

    float rotation = 0.0f;
public:
    void init() override {

        renderer = &opengl;

        Mesh cube{
            {
                // Front
                { -0.5f, -0.5f,  0.5f,  1, 0, 0,  0, 0 },
                {  0.5f, -0.5f,  0.5f,  0, 1, 0,  1, 0 },
                {  0.5f,  0.5f,  0.5f,  0, 0, 1,  1, 1 },
                { -0.5f,  0.5f,  0.5f,  1, 1, 0,  0, 1 },

                // Back
                { -0.5f, -0.5f, -0.5f,  1, 0, 1,  0, 0 },
                {  0.5f, -0.5f, -0.5f,  0, 1, 1,  1, 0 },
                {  0.5f,  0.5f, -0.5f,  1, 1, 1,  1, 1 },
                { -0.5f,  0.5f, -0.5f,  0, 0, 0,  0, 1 },
            },
            {
                0, 1, 2,  2, 3, 0,
                1, 5, 6,  6, 2, 1,
                5, 4, 7,  7, 6, 5,
                4, 0, 3,  3, 7, 4,
                3, 2, 6,  6, 7, 3,
                4, 5, 1,  1, 0, 4
            }
        };

        mesh_id = renderer->add_mesh(cube);

        Shader shader{
            "res/shaders/triangle.vert",
            "res/shaders/triangle.frag"
        };

        shader_id = renderer->add_shader(shader);
    }


    void tick(const f32& dt) override {
        rotation += dt; // accumulate rotation
    }

    void render() override {
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0,1,0));

        DrawCall call{};
        call.mesh_id = mesh_id;
        call.shader_id = shader_id;

        // dynamic transform uniforms
        call.mat4_uniforms["transform"] = proj * view * model;

        // you can add more dynamically
        // glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
        // call.mat4_uniforms["scale"] = scale;

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