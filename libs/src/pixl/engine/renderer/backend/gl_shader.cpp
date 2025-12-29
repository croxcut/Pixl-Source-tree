#include "pixl/engine/renderer/backend/gl_shader.h"

#include "pixl/utility/file.h"

void GLShader::create_shader(struct ShaderSource& shader) {
    vertex = compile(
        shader.vertex, 
        GL_VERTEX_SHADER);

    fragment = compile(
        shader.fragment, 
        GL_FRAGMENT_SHADER);

    program = create_program(
        vertex, 
        fragment);
}

u32 GLShader::compile(const char* source, GLenum type) {
    const char* _source = load_shader_file(source);
    
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

