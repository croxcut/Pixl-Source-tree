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

#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "misc/utility/log.h"

namespace file{
    inline const char* load_shader(const char* shaderFile) {
        std::ifstream shader_t(shaderFile);
        ASSERT(shader_t, "Failed to Open Shader File at: %s", shaderFile);

        std::string content(
            (std::istreambuf_iterator<char>(shader_t)),
            std::istreambuf_iterator<char>()
        );

        char* buffer = new char[content.size() + 1];
        std::memcpy(buffer, content.c_str(), content.size() + 1);
        return buffer; 
    }
}

#endif