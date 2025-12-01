#include "Shader.h"

#define INFO_LOG_BUFFER 512

Shader::Shader(
    const char* vertexShaderPath,
    const char* fragmentShaderPath
) {
    
    const char* vertexShaderSource = loadShaderFromFile(vertexShaderPath).c_str();
    const char* fragmentShaderSource = loadShaderFromFile(fragmentShaderPath).c_str();

    createShader(vertexShaderSource, &vertexShader, GL_VERTEX_SHADER);
    createShader(fragmentShaderSource, &fragmentShader, GL_FRAGMENT_SHADER);

    createProgram();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::createShader(
    const char* shaderSource, 
    u32 *shader, 
    GLuint shaderType
) {
    *shader = glCreateShader(shaderType);
    glShaderSource(*shader, 1, &shaderSource, NULL);
    compile(shader);
}

void Shader::compile(
    u32 *shader
) {
    int success = 0;
    char infoLog[INFO_LOG_BUFFER];

    glCompileShader(*shader);
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(*shader, 512, NULL, infoLog);
        LOG(ERROR, "Error Compiling Shader: %s", infoLog);
    }
}

void Shader::createProgram() {
    int success = 0;
    char infoLog[INFO_LOG_BUFFER];
    
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, vertexShader);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        LOG(ERROR, "Error Linking Shader: %s", infoLog);
    }
}

void Shader::use() {
    glUseProgram(programID);
}

void Shader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value); 
} 