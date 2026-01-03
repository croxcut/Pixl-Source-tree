#include "pixl/engine/camera/camera.h"

glm::mat4 Camera::get_view_matrix() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::lookAt(position, position + glm::normalize(front), glm::vec3(0,1,0));
}

void Camera::process_keyboard(Window* window, float dt) {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = 0; // ignore Y for horizontal movement
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0,1,0)));

    if(window->key_down(GLFW_KEY_W)) position += front * speed * dt;
    if(window->key_down(GLFW_KEY_S)) position -= front * speed * dt;
    if(window->key_down(GLFW_KEY_A)) position -= right * speed * dt;
    if(window->key_down(GLFW_KEY_D)) position += right * speed * dt;
    if(window->key_down(GLFW_KEY_SPACE)) position.y += speed * dt;
    if(window->key_down(GLFW_KEY_LEFT_SHIFT)) position.y -= speed * dt;
}

void Camera::process_mouse(float xoffset, float yoffset) {
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
}