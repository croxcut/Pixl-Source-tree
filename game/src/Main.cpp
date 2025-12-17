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
                "res/texture/0x1.jpg",
            }
        };

        squareMeshId = renderer->createMesh(squareMesh);

        Shader colorShader = {
            "res/shader/vertex.glsl",
            "res/shader/fragment.glsl"
        };
        colorShaderId = renderer->createShader(colorShader);

        SceneObject obj;
        obj.meshId = squareMeshId;
        obj.position = glm::vec3(0.0f, 0.0f, 0.0f);
        obj.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        obj.scale = glm::vec3(1.0f, 1.0f, 1.0f);
        objects.push_back(obj);

        selectedObject = 0;

        renderer->init();

    };

    void tick() override {
        renderer->tick();


    };

    void draw() override {
        for (auto& obj : objects) {
            glm::mat4 transform = glm::mat4(1.0f);

            transform = glm::translate(transform, obj.position);

            transform = glm::rotate(transform, glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            transform = glm::rotate(transform, glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::rotate(transform, glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            transform = glm::scale(transform, obj.scale);
            
            renderer->submitDrawCall(obj.meshId, colorShaderId, transform);
        }

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
    Window& window = Window::createWindow(zvezda);
    window.start();
    return 0;
}
