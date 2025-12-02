#include "GLShader.h"

#define INFO_LOG_BUFFER 512

GLShader::GLShader(
    const char* vertexShaderPath,
    const char* fragmentShaderPath
) {
    LOG(INFO, "Initializing Shader...");


    const char* vertexShaderSource = loadShaderFromFile(vertexShaderPath);
    const char* fragmentShaderSource = loadShaderFromFile(fragmentShaderPath);

    createShader(vertexShaderSource, &vertexShader, GL_VERTEX_SHADER);
    createShader(fragmentShaderSource, &fragmentShader, GL_FRAGMENT_SHADER);

    createProgram();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    delete[] vertexShaderSource;
    delete[] fragmentShaderSource;

    LOG(INFO, "Initializing Shader Done!");
}

void GLShader::createShader(
    const char* shaderSource, 
    u32 *shader, 
    GLuint shaderType
) {
    LOG(INFO, "Creating Shader... %s", shaderSource);
    *shader = glCreateShader(shaderType);
    glShaderSource(*shader, 1, &shaderSource, NULL);
    compile(shader);
    LOG(INFO, "Shader Created!");
}

void GLShader::compile(
    u32 *shader
) {
    LOG(INFO, "Compiling Shader...");
    int success = 0;
    char infoLog[INFO_LOG_BUFFER];

    glCompileShader(*shader);
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(*shader, 512, NULL, infoLog);
        LOG(ERROR, "Error Compiling Shader: %s", infoLog);
    }
    LOG(INFO, "Shader Compiled!");
}

void GLShader::createProgram() {
    LOG(INFO, "Creating Program...");
    
    int success = 0;
    char infoLog[INFO_LOG_BUFFER];
    
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        LOG(ERROR, "Error Linking Shader: %s", infoLog);
    }
    LOG(INFO, "Program Created");
}

void GLShader::use() {
    glUseProgram(programID);
}

void GLShader::cleanup() {
    glDeleteProgram(programID);
}

void GLShader::setBool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value); 
}
void GLShader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value); 
}
void GLShader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value); 
} 