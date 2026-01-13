#include "engine.h"

Engine::Engine(IAppLogic& applogic) :
    applogic(&applogic) {
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

    applogic->init();
}

void Engine::tick(const f32& dt) {
    applogic->tick(dt);
}

void Engine::render() {
    applogic->render();
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
    applogic->cleanup();
}