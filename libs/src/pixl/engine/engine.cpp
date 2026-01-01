#include <pixl/engine/engine.h>

#include <chrono>

Engine::Engine(IAppLogic& applogic)
    : applogic(&applogic) 
{
    window = Window::create_window();   
    WindowOpts opts;
    opts.title = "Pixl Engine v0.1.15";
    opts.width = 1280;
    opts.height = 720;

    window->set_opts(opts);     
    window->init();             
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

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glEnable(GL_DEPTH_TEST);    
    glEnable(GL_CULL_FACE);     
    glCullFace(GL_BACK);        
    glFrontFace(GL_CCW);        

    using clock = std::chrono::high_resolution_clock;
    constexpr float dt = 1.0f / 60.0f;
    float accumulator = 0.0f;
    auto current_time = clock::now();
    float fps_timer = 0.0f;
    int frames = 0, ticks = 0;
    
    while (running) {
        auto new_time = clock::now();
        float delta_time = std::chrono::duration<float>(new_time - current_time).count();
        current_time = new_time;
        accumulator += delta_time;
        fps_timer += delta_time;
        
        window->poll_events();
        if (window->close()) running = false;

        while(accumulator >= dt) {
            tick(dt);
            accumulator -= dt;
            ticks++;    
        }

        render();
        frames++;

        window->refresh();

        if(fps_timer >= 1.0f) {
            LOG("FPS: %d | TPS: %d", frames, ticks);
            frames = 0;
            ticks = 0;
            fps_timer -= 1.0f;
        }
    }
}

void Engine::cleanup() {
    applogic->cleanup();
}