/*==============================================================================
    PIXL ENGINE

    Copyright (C) 2026 John Paul Valenzuela
    Author : John Paul Valenzuela
    Email  : johnpaulvalenzuelaog@gmail.com

    This file is part of PIXL ENGINE.

    PIXL ENGINE is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PIXL ENGINE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PIXL ENGINE.  If not, see <https://www.gnu.org/licenses/>.
==============================================================================*/

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