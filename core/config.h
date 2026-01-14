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

#ifndef CONFIG_H
#define CONFIG_H

// #define PIXL_LOGGER_ENABLED

#include "version.h"

#define PIXL_RENDERER_BACKEND_OPENGL
#define PIXL_RENDERER_BACKEND_VULKAN
#define PIXL_RENDERER_BACKEND_DX3D12

#define ENGINE_TITLE \
    "Pixl Engine v" STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_PATCH)

struct WindowConfig {
    int width = 1280;
    int height = 720;
    const char* title = ENGINE_TITLE;
};

#endif
