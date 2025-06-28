#include <stdio.h>


#include "./engine/Window.h"

void init() {

}

void update() {

}

void render() {

}

void cleanup() {

}

int main(int argc, char* argv[]) {

    window_create(init, update, render, cleanup);
    loop();

    return 0;
}


