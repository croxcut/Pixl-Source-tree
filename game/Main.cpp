
#include "../pixl/src/engine/Window.h"
#include "../pixl/src/engine/gfx/renderer/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION

class Main : public IAppLogic{

private:

    Renderer& renderer = Renderer::get();

public:

    void init() override {
        renderer.init(GraphicsApi::Opengl);
    };

    void tick() override {

    };

    void render() override {
        renderer.draw();
    };

    void cleanup() override {
        renderer.cleanup();
    }

};

int main(int argc, char* argv[]) {

    Main main;
    Window window = Window::createWindow(main);
    window.start();

    return 0;
}


