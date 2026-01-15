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

#include "pxl_arena.h"
#include "pxl_memory.h"

namespace px {

arena* create_arena(size_t size) {
    arena* a = (arena*)malloc(sizeof(arena));
    a->base = (char*)malloc(size);
    a->size = size;
    a->offset = 0;
    return a;
}

void* arena_alloc(arena* a, size_t size) {
    size = (size + 15) & ~15;
    if (a->offset + size > a->size)
        return nullptr;
    void* p = a->base + a->offset;
    a->offset += size;
    return p;
}

void arena_reset(arena* a) {
    a->offset = 0;
}

}