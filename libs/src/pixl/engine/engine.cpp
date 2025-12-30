#include <pixl/engine/engine.h>

Engine::Engine(IAppLogic& applogic)
    : applogic(&applogic) 
{
    WindowOpts opts;
    window = Window::create_window(opts);
    LOG("Engine Created");    
}

Engine::~Engine() {
    
}

void Engine::init() {
    applogic->init();
    LOG("Engine started");
}

void Engine::start() {
    running = true;
    LOG("Engine running");
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

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    float dt = 0.0f;

    while (running) {
        window->poll_events();
        if (window->close()) break;

        tick(dt);
        render();
        window->refresh();
    }
}

void Engine::cleanup() {

}