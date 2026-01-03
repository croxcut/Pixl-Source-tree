#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "pixl/window/window.h"

class Camera {
public:
    glm::vec3 position;
    float pitch; // rotation around X axis
    float yaw;   // rotation around Y axis
    float speed;
    float sensitivity;

    Camera(glm::vec3 start_pos = glm::vec3(0, 0, 3))
        : position(start_pos), pitch(0.0f), yaw(-90.0f), speed(2.5f), sensitivity(0.1f)
    {  /*Initializer*/  }

    glm::mat4 get_view_matrix();

    void process_keyboard(Window* window, float dt);

    void process_mouse(float xoffset, float yoffset);
};

#endif