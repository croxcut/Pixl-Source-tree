#include <iostream>

#include <pixl/window/window.h>

int main(int argc, char* argv[]) {

    WindowOpts opts;
    Window* window = Window::create_window(opts);

    while(!window->close()) {
        window->poll_events();
        window->refresh();
    }
    
    return 0;
}