#include <iostream>

#include <pixl/engine/core/engine.h>
#include <pixl/engine/camera/camera.h>
#include <pixl/engine/renderer/backend/opengl.h>
#include <pixl/engine/renderer/i_renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <random>

struct Frustum {
    glm::vec4 planes[6]; // left, right, bottom, top, near, far

    // extract frustum planes from projection * view matrix
    void update(const glm::mat4& pv) {
        // Left
        planes[0] = glm::row(pv, 3) + glm::row(pv, 0);
        // Right
        planes[1] = glm::row(pv, 3) - glm::row(pv, 0);
        // Bottom
        planes[2] = glm::row(pv, 3) + glm::row(pv, 1);
        // Top
        planes[3] = glm::row(pv, 3) - glm::row(pv, 1);
        // Near
        planes[4] = glm::row(pv, 3) + glm::row(pv, 2);
        // Far
        planes[5] = glm::row(pv, 3) - glm::row(pv, 2);

        // normalize planes
        for(int i = 0; i < 6; ++i) {
            planes[i] /= glm::length(glm::vec3(planes[i]));
        }
    }

    // sphere: center and radius
    bool is_sphere_visible(const glm::vec3& center, float radius) const {
        for(int i = 0; i < 6; ++i) {
            if(glm::dot(glm::vec3(planes[i]), center) + planes[i].w + radius < 0)
                return false;
        }
        return true;
    }
};

class App : public IAppLogic {
private:
    IRenderer* renderer = nullptr;           
    u64 mesh_id = 0;
    u64 shader_id = 0;
    OpenGL opengl;

    struct CubeInstance {
        glm::vec3 position;
        glm::vec3 rotation; 
    };

    std::vector<CubeInstance> cubes;

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

        // Generate 100+ cubes with random positions and rotations
        std::mt19937 rng((unsigned int)time(nullptr));
        std::uniform_real_distribution<float> pos_dist(-20.0f, 20.0f);
        std::uniform_real_distribution<float> rot_dist(0.0f, 360.0f);

        for (int i = 0; i < 999; ++i) {
            cubes.push_back({
                glm::vec3(pos_dist(rng), pos_dist(rng), pos_dist(rng)),
                glm::vec3(rot_dist(rng), rot_dist(rng), rot_dist(rng))
            });
        }
    }

    float log_timer = 0.0f;
    void tick(const f32& dt) override {
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
        glm::mat4 pv = proj * view;

        // update frustum once per frame
        Frustum frustum;
        frustum.update(pv);

        const float cube_radius = 0.87f; // sqrt(3)/2 for unit cube diagonal

        for (auto& cube : cubes) {
            if(!frustum.is_sphere_visible(cube.position, cube_radius))
                continue; // skip cubes outside frustum

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube.position);
            model = glm::rotate(model, glm::radians(cube.rotation.x), glm::vec3(1,0,0));
            model = glm::rotate(model, glm::radians(cube.rotation.y), glm::vec3(0,1,0));
            model = glm::rotate(model, glm::radians(cube.rotation.z), glm::vec3(0,0,1));

            DrawCall call{};
            call.mesh_id = mesh_id;
            call.shader_id = shader_id;
            call.mat4_uniforms["transform"] = pv * model;

            renderer->submit_draw_call(call);
        }

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