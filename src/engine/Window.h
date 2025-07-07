#ifndef WINDOW_H
#define WINDOW_H

#include "./util/Util.h"

typedef void (*FWindow)();


struct Window{
    GLFWwindow* handle;
    FWindow init, render, update, cleanup;
};

extern struct Window window;


void window_create(FWindow init, FWindow render, FWindow update, FWindow cleanup);
void window_loop();

#endif