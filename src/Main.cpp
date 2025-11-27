
#include "engine/Window.h"

#define STB_IMAGE_IMPLEMENTATION

class Main : public IAppLogic{

    void init() override {
        std::cout << "Initializing" << std::endl;
    };

    void tick() override {

    };

    void render() override {
        std::cout << "Rendering" << std::endl;
    };

    void cleanup() override {
        std::cout << "Exiting" << std::endl;
    }

};

int main(int argc, char* argv[]) {

    Main main;
    Window window = Window::createWindow(main);
    window.start();

    return 0;
}


