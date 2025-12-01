#include "OpenGLDevice.h"

// float vertices[] = {
//     -0.5f, -0.5f, 0.0f,
//      0.5f, -0.5f, 0.0f,
//      0.0f,  0.5f, 0.0f
// };

float vertices[] = {
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 5.0f, 0.5f, 0.0f, 
};

unsigned int indices[] = {  
    0, 1, 3,  
    1, 2, 3   
};  

const char *vertexShaderSource = "#version 460 core\n"
                        "layout (location = 0) in vec3 aPos;\n"
                        "layout (location = 1) in vec3 aColor;\n"
                        "out vec3 ourColor;\n"
                        "void main()\n"
                        "{\n"
                        "   gl_Position = vec4(aPos, 1.0);\n"
                        "   ourColor = aColor;\n"
                        "}\0";

const char *fragmentShaderSource = "#version 460 core\n"
                        "out vec4 FragColor;\n"
                        "in vec3 ourColor;\n"
                        "void main()\n"
                        "{\n"
                        "   FragColor = vec4(ourColor, 1.0);\n"
                        "}\0";

OpenGLDevice& OpenGLDevice::get() {
    static OpenGLDevice instance;
    return instance;
}

OpenGLDevice::OpenGLDevice() {
    init();
}

void OpenGLDevice::init() {
    LOG(INFO, "OpenGL Intialized!");

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

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void OpenGLDevice::createTexture() {


}

void OpenGLDevice::draw() {
    glUseProgram(shaderProgram);

    float timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OpenGLDevice::cleanup() {

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}