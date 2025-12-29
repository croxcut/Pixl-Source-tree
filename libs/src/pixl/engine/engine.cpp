#include <pixl/engine/engine.h>

Engine::Engine(IAppLogic& applogic)
    : applogic(&applogic) 
{
    WindowOpts opts;
    window = Window::create_window(opts);
    
}

Engine::~Engine() {
    
}

void Engine::init() {
    applogic->init();
}

void Engine::start() {
    running = true;
    run();
}

void Engine::stop() {

}

void Engine::render() {
    applogic->render();
}
 
void Engine::tick(const f32& dt) {
    applogic->tick(dt);
}

void Engine::run() {
    init();

    float dt;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while(running && !window->close()) {
        tick(dt);

        glClear(GL_COLOR_BUFFER_BIT);
        render();

        window->poll_events();
        window->refresh();
    }
}

void Engine::cleanup() {

}