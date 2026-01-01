#include <iostream>

#include <pixl/engine/engine.h>
#include <pixl/engine/camera.h>
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

    Camera camera;
    Window* window = nullptr;
    bool first_mouse = true;
    double last_x = 0.0, last_y = 0.0;

public:
    void init() override {
        renderer = &opengl;
        window = Window::create_window();

        // Capture mouse
        glfwSetInputMode(window->get_handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Cube mesh & shader
        Mesh cube{
            {
                { -0.5f, -0.5f,  0.5f,  1,0,0, 0,0 },
                {  0.5f, -0.5f,  0.5f,  0,1,0, 1,0 },
                {  0.5f,  0.5f,  0.5f,  0,0,1, 1,1 },
                { -0.5f,  0.5f,  0.5f,  1,1,0, 0,1 },
                { -0.5f, -0.5f, -0.5f,  1,0,1, 0,0 },
                {  0.5f, -0.5f, -0.5f,  0,1,1, 1,0 },
                {  0.5f,  0.5f, -0.5f,  1,1,1, 1,1 },
                { -0.5f,  0.5f, -0.5f,  0,0,0, 0,1 },
            },
            {
                0,1,2, 2,3,0,
                1,5,6, 6,2,1,
                5,4,7, 7,6,5,
                4,0,3, 3,7,4,
                3,2,6, 6,7,3,
                4,5,1, 1,0,4
            }
        };

        mesh_id = renderer->add_mesh(cube);

        Shader shader{"res/shaders/triangle.vert", "res/shaders/triangle.frag"};
        shader_id = renderer->add_shader(shader);
    }

    void tick(const f32& dt) override {
        rotation += dt;

        // Camera movement
        camera.process_keyboard(window, dt);

        // Mouse movement
        double xpos, ypos;
        glfwGetCursorPos(window->get_handle(), &xpos, &ypos);
        if(first_mouse) { last_x = xpos; last_y = ypos; first_mouse = false; }

        float xoffset = xpos - last_x;
        float yoffset = last_y - ypos; // reversed: y ranges bottom->top
        last_x = xpos;
        last_y = ypos;

        camera.process_mouse(xoffset, yoffset);
    }

    void render() override {
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.get_view_matrix();
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0,1,0));

        DrawCall call{};
        call.mesh_id = mesh_id;
        call.shader_id = shader_id;
        call.mat4_uniforms["transform"] = proj * view * model;

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