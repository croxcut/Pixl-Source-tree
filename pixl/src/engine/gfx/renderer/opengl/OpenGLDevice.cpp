#include "OpenGLDevice.h"

// float vertices[] = {
//     -0.5f, -0.5f, 0.0f,
//      0.5f, -0.5f, 0.0f,
//      0.0f,  0.5f, 0.0f
// };

float vertices[] = {
     0.5f,  0.5f, 0.0f,  
     0.5f, -0.5f, 0.0f,  
    -0.5f,  0.5f, 0.0f,  

     0.5f, -0.5f, 0.0f,  
    -0.5f, -0.5f, 0.0f,  
    -0.5f,  0.5f, 0.0f   
}; 

const char *vertexShaderSource = "#version 460 core\n"
                        "layout (location = 0) in vec3 aPos;\n"
                        "void main()\n"
                        "{\n"
                        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                        "}\0";

const char *fragmentShaderSource = "#version 460 core\n"
                        "out vec4 FragColor;\n"
                        "void main()\n"
                        "{\n"
                        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                        "}\0";

OpenGLDevice& OpenGLDevice::get() {
    static OpenGLDevice instance;
    return instance;
}

OpenGLDevice::OpenGLDevice() {
    init();
}

void OpenGLDevice::init() {
    Log(INFO, "OpenGL Intialized!");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


}

void OpenGLDevice::createTexture() {


}

void OpenGLDevice::draw() {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void OpenGLDevice::cleanup() {

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}