#ifndef GL41_SHADER_LOADER_H
#define GL41_SHADER_LOADER_H

#include "misc/utility/types.h"
#include "backend/renderer_backend.h"

#include <glad/glad.h>

class GL41Shader {

private:
    u32 vertex;
    u32 fragment;
    u32 program;

public:
    GL41Shader(struct Shader& shader);

    void use();
    void clear();

    void set_mat4(const char* uniform, const glm::mat4& mat);

private:

    u32 compile(const char* source, GLenum type);
    u32 create_program(const u32& vertex, const u32& fragment);

};

#endif