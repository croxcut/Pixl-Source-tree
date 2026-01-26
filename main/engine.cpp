/*********************************************************************************
*                                                                                *
*                                PIXL ENGINE                                     *
*                                                                                *
*  Copyright (c) 2025-present John Paul Valenzuela                               *
*                                                                                *
*  MIT License                                                                   *
*                                                                                *
*  Permission is hereby granted, free of charge, to any person obtaining a copy  *
*  of this software and associated documentation files (the "Software"), to      *
*  deal in the Software without restriction, including without limitation the    *
*  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or   *
*  sell copies of the Software, and to permit persons to whom the Software is    *
*  furnished to do so, subject to the following conditions:                      *
*                                                                                *
*  The above copyright notice and this permission notice shall be included in    *
*  all copies or substantial portions of the Software.                           *
*                                                                                *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL       *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER    *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
*  THE SOFTWARE.                                                                 *
*                                                                                * 
**********************************************************************************/

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

void Engine::tick(const f32_t& dt) {
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