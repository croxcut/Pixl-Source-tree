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

#ifndef PXL_RENDERER_H
#define PXL_RENDERER_H

#include "backend/pxL_renderer_backend.h"

class PXLRenderer {
public:
    virtual u64 add_mesh(struct Mesh& mesh) = 0;
    virtual u64 add_shader(struct Shader& shader) = 0;
    virtual void submit_draw_call(const struct DrawCall& draw_call) = 0;
    virtual void draw() = 0;
    virtual void cleanup() = 0;
};

#endif