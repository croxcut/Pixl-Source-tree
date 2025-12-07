#include <pixl/engine/gfx/Window.h>
#include <pixl/engine/gfx/renderer/Renderer.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

class Zvezda : public IAppLogic {

private:

    Assimp::Importer importer;

    IRenderer* renderer = nullptr;

    // mesh + shader ids
    std::string squareMeshId;
    std::string colorShaderId;

    // scene objects and selection (IAppLogic requires these)
    std::vector<SceneObject> objects;
    int selectedObject = -1;

public:

    // IAppLogic interface for editor
    std::vector<SceneObject>& getSceneObjects() override {
        return objects;
    }

    int& getSelectedObjectIndex() override {
        return selectedObject;
    }

    void init() override {
        renderer = Renderer::getInstance(OPENGL);

        // create a simple square mesh (same as original)
        Mesh squareMesh = {
            {
                // vertex: pos.x, pos.y, pos.z,  normal.x, normal.y, normal.z,  u, v
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

        // create a SceneObject for the square and select it
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

        // For demonstration: rotate all objects slowly (you can disable this if you want)
        float time = (float)glfwGetTime();
        // apply transforms from SceneObject to the renderer draw calls
        for (auto& obj : objects) {
            glm::mat4 transform = glm::mat4(1.0f);

            transform = glm::translate(transform, obj.position);
            // rotation stored as degrees in obj.rotation.
            // rotate X, Y, Z (Z-last keeps 2D rotation intuitive)
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
