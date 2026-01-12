#include <iostream>

#include "core/window/window.h"

int main(int argc, char* argv[]) {

    Window* window = Window::create_window();
    
    WindowConfig config;    
    window->setup_window_config(config);
    window->init();

    // Test
    while(!window->close()) {
        window->poll_events();
        window->refresh();
    }

    return 0;
}