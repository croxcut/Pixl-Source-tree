
#include "engine/Window.h"
#include "engine/gfx/renderer/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION

class Main : public IAppLogic{

public:

    void init() override {
        std::cout << "Initializing" << std::endl;

        _renderer.init(GraphicsApi::Opengl);
    };

    void tick() override {

    };

    void render() override {
        // std::cout << "Rendering" << std::endl;

        _renderer.draw();
    };

    void cleanup() override {
        std::cout << "Exiting" << std::endl;
        _renderer.cleanup();
    }

};

int main(int argc, char* argv[]) {

    Main main;
    Window window = Window::createWindow(main);
    window.start();

    return 0;
}


