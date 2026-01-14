/*==============================================================================
    PIXL ENGINE

    Copyright (C) 2026 John Paul Valenzuela
    Author : John Paul Valenzuela
    Email  : johnpaulvalenzuelaog@gmail.com

    This file is part of PIXL ENGINE.

    PIXL ENGINE is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PIXL ENGINE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PIXL ENGINE.  If not, see <https://www.gnu.org/licenses/>.
==============================================================================*/

#include "gl41_shader.h"

#include <glm/gtc/type_ptr.hpp>

#include "core/io/file.h"

/**
 *      @param Shader
 *  
 * 
 *      
 */
GL41Shader::GL41Shader(struct Shader& shader) {
    program = create_program(
        compile(
            shader.vertex,
            GL_VERTEX_SHADER),
        compile(
            shader.fragment,
            GL_FRAGMENT_SHADER)
    );
}

u32 GL41Shader::compile(
    const char* source,
    GLenum type
) {
    const char* _source = file::load_shader(source);

    u32 _shader = 0;
    _shader = glCreateShader(type);
    glShaderSource(_shader, 1, &_source, NULL);
    glCompileShader(_shader);

    delete[] _source;
    return _shader;
}

u32 GL41Shader::create_program(
    const u32& vertex,
    const u32& fragment
) {
    u32 _program = 0;

    _program = glCreateProgram();
    glAttachShader(_program, vertex);
    glAttachShader(_program, fragment);
    glLinkProgram(_program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return _program;
}

void GL41Shader::use() {
    glUseProgram(program);
}

void GL41Shader::clear() {
    glUseProgram(0);
}

void GL41Shader::set_mat4(
    const char* uniform,
    const glm::mat4& mat
) {
    u32 loc = glGetUniformLocation(program, uniform);
    if(loc != -1) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
    }
}