#include <pixl/engine/gfx/Window.h>
#include <pixl/engine/gfx/renderer/renderer.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

#include "scene/MainScene.h"

class Zvezda : public IAppLogic {

private:


public:

    void init() override {

    };

    void tick() override {

    };

    void draw() override {

    };

    void cleanup() override {

    }

};

int main(int argc, char* argv[]) {
    Zvezda zvezda;
    Window& window = Window::createWindow(zvezda);
    window.start();
    return 0;
}
