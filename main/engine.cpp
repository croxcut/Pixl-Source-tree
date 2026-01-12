#include "engine.h"

Engine::Engine() {

}

Engine::~Engine() {

}

void Engine::start() {
    run();
}

void Engine::stop() {

}

void Engine::init() {
    window = Window::create_window();

    WindowConfig config;    
    window->setup_window_config(config);
    window->init();
}

void Engine::tick(const f32& dt) {

}

void Engine::render() {

}

void Engine::run() {
    init();

    while(true) {
        if(window->close()) break;

        window->poll_events();
        window->refresh();        
    }

    cleanup();
}

void Engine::cleanup() {

}