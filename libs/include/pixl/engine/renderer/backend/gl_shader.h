#ifndef GL_SHADER_H
#define GL_SHADER_H

#include "../render_objects.h"

#include <glad/glad.h>

class GLShader {

private:
    u32 vertex, fragment;
    u32 program;

public:

    GLShader(struct Shader& shader);

    void use();
    void clear();

private:

    u32 compile(const char* source, GLenum type);
    u32 create_program(const u32& vertex, const u32& fragment);

};

#endif