#include <pixl/engine/gfx/window.h>
#include <pixl/engine/gfx/renderer/renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

#include "scene/MainScene.h"

class Zvezda : public IAppLogic {

public:

private:
    IRenderer* renderer = nullptr;

    u64 squareMeshId;
    u64 colorShaderId;

    std::vector<SceneObject> objects;
    int selectedObject = -1;

public:

    void init() override {

        renderer = Renderer::getInstance(OPENGL);

        Mesh cubeMesh = {
            {
                { -0.5f, -0.5f,  0.5f,  1,0,0,  0,0 },
                {  0.5f, -0.5f,  0.5f,  1,0,0,  1,0 },
                {  0.5f,  0.5f,  0.5f,  1,0,0,  1,1 },
                { -0.5f,  0.5f,  0.5f,  1,0,0,  0,1 },

                {  0.5f, -0.5f, -0.5f,  0,1,0,  0,0 },
                { -0.5f, -0.5f, -0.5f,  0,1,0,  1,0 },
                { -0.5f,  0.5f, -0.5f,  0,1,0,  1,1 },
                {  0.5f,  0.5f, -0.5f,  0,1,0,  0,1 },

                { -0.5f, -0.5f, -0.5f,  0,0,1,  0,0 },
                { -0.5f, -0.5f,  0.5f,  0,0,1,  1,0 },
                { -0.5f,  0.5f,  0.5f,  0,0,1,  1,1 },
                { -0.5f,  0.5f, -0.5f,  0,0,1,  0,1 },

                {  0.5f, -0.5f,  0.5f,  1,1,0,  0,0 },
                {  0.5f, -0.5f, -0.5f,  1,1,0,  1,0 },
                {  0.5f,  0.5f, -0.5f,  1,1,0,  1,1 },
                {  0.5f,  0.5f,  0.5f,  1,1,0,  0,1 },

                { -0.5f,  0.5f,  0.5f,  1,0,1,  0,0 },
                {  0.5f,  0.5f,  0.5f,  1,0,1,  1,0 },
                {  0.5f,  0.5f, -0.5f,  1,0,1,  1,1 },
                { -0.5f,  0.5f, -0.5f,  1,0,1,  0,1 },

                { -0.5f, -0.5f, -0.5f,  0,1,1,  0,0 },
                {  0.5f, -0.5f, -0.5f,  0,1,1,  1,0 },
                {  0.5f, -0.5f,  0.5f,  0,1,1,  1,1 },
                { -0.5f, -0.5f,  0.5f,  0,1,1,  0,1 },
            },
            {
                 0, 1, 2,  2, 3, 0,
                 4, 5, 6,  6, 7, 4,
                 8, 9,10, 10,11, 8,
                12,13,14, 14,15,12,
                16,17,18, 18,19,16,
                20,21,22, 22,23,20
            },
            {
                "res/texture/0x1.jpg"
            }
        };

        squareMeshId = renderer->createMesh(cubeMesh);

        Shader colorShader = {
            "res/shader/vertex.glsl",
            "res/shader/fragment.glsl"
        };
        colorShaderId = renderer->createShader(colorShader);

        SceneObject obj;
        obj.meshId = squareMeshId;
        obj.position = glm::vec3(0.0f, 0.0f, 0.0f);
        objects.push_back(obj);

        selectedObject = 0;

        renderer->init();

    };

    void tick() override {
        renderer->tick();


    };

    void draw() override {

        glm::mat4 view = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(0.0f, 0.0f, -3.0f)
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            800.0f / 600.0f,
            0.1f,
            100.0f
        );

        renderer->setViewProjection(view, projection);

        for (auto& obj : objects) {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, obj.position);
            transform = glm::rotate(
                transform,
                (float)glfwGetTime(),
                glm::vec3(0.5f, 1.0f, 0.0f)
            );

            renderer->submitDrawCall(
                obj.meshId,
                colorShaderId,
                transform
            );
        }

        renderer->draw();
    }

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
    Window& window = Window::createWindow(zvezda);
    window.start();
    return 0;
}
