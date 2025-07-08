#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION

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
      0.5f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 
      0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
     -0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
     -0.5f, -0.5f, 0.0f,    0.3f, 0.3f, 3.0f,    0.0f, 0.0f,
};

// unsigned int indices[] = {
//     0, 1, 2,
//     3, 4, 5
// };

unsigned int indices[] = {
    0, 1, 2,
    3, 2, 1
};


float textCoords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f
};

unsigned int VBO, VAO;
unsigned int EBO;

unsigned int texture;

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    shader = shader_create("./res/shader/vert.glsl", "./res/shader/frag.glsl");
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    stbi_set_flip_vertically_on_load(true);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("./res/textures/0x1.jpg", &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(true);  
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load texture\n");
    }

    stbi_image_free(data);
}

void update() {

}

void render() { 
    
    shader_use(&shader);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    
}

void cleanup() {
    shader_delete(&shader);
}

int main(int argc, char* argv[]) {

    window_create(init, update, render, cleanup);
    window_loop();

    return 0;
}


