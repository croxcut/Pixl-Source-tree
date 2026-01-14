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

#ifndef PXL_MEMORY_ALLOC_H
#define PXL_MEMORY_ALLOC_H

#include <cstddef>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#endif

void* px_malloc(size_t size);
void px_free(void* ptr);
void* px_realloc(void* ptr, size_t new_size);

#endif 