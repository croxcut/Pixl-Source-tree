#include <GL/gl.h>
#include <cmath>

extern "C" __declspec(dllexport) void game_update(float dt) {
    static float t = 0.0f;
    t += dt;

    float r = 0.5f + 0.5f * sin(t);
    float g = 0.5f + 0.5f * sin(t * 1.3f);
    float b = 0.5f + 0.5f * sin(t * 1.7f);

    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
