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

#ifndef GL41_SHADER_LOADER_H
#define GL41_SHADER_LOADER_H

#include "misc/utility/types.h"
#include "backend/pxL_renderer_backend.h"

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