#include "pixl/engine/renderer/backend/gl_shader.h"

#include <glm/gtc/type_ptr.hpp>
#include "pixl/utility/file.h"

GLShader::GLShader(struct Shader& shader) {
    program = create_program(
        compile(
            shader.vertex, 
            GL_VERTEX_SHADER), 
        compile(
            shader.fragment, 
            GL_FRAGMENT_SHADER)
        );
}

u32 GLShader::compile(const char* source, GLenum type) {
    const char* _source = file::load_shader(source);
    
    u32 _shader = 0;
    _shader = glCreateShader(type);
    glShaderSource(_shader, 1, &_source, NULL);
    glCompileShader(_shader);
    
    delete[] _source;
    
    return _shader;
}

u32 GLShader::create_program(const u32& vertex, const u32& fragment) {
    u32 _program = 0;

    _program = glCreateProgram();
    glAttachShader(_program, vertex);   
    glAttachShader(_program, fragment);
    glLinkProgram(_program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return _program;
}                       

void GLShader::use() {
    glUseProgram(program);
}

void GLShader::clear() {
    glUseProgram(0);
}            

void GLShader::set_mat4(const char* name, const glm::mat4& mat) {
    GLint loc = glGetUniformLocation(program, name);
    if (loc != -1) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
}