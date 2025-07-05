#include <stdio.h>


#include "./engine/Window.h"
#include "./engine/gfx/Shader.h"

// float vertices[] = {
//     -0.5f, -0.5f, 0.0f,
//      0.5f, -0.5f, 0.0f,
//      0.0f,  0.5f, 0.0f,
// };

// float vertices[] = {
//      0.5f,  0.5f, 0.0f,
//      0.5f, -0.5f, 0.0f,
//     -0.5f,  0.5f, 0.0f,

//      0.5f, -0.5f, 0.0f,
//     -0.5f, -0.5f, 0.0f,
//     -0.5f,  0.5f, 0.0f,     
// };

float vertices[] = {
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,

     0.45f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 
    -0.55f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,       
    -0.55f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
};

unsigned int indices[] = {
    0, 1, 2,
    3, 4, 5
};

unsigned int VBO, VAO;
unsigned int EBO;

struct Shader shader;

void init() {
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader = shader_create("./res/shader/shader.vert", "./res/shader/shader.frag");
    
}

void update() {

}

void render() { 
    
    shader_use(&shader);
    shader_uniform_setFloat(&shader, "someuniform", 1.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void cleanup() {
    shader_delete(&shader);
}

int main(int argc, char* argv[]) {

    window_create(init, update, render, cleanup);
    loop();

    return 0;
}


