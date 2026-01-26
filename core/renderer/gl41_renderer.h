/*********************************************************************************
*                                                                                *
*                                PIXL ENGINE                                     *
*                                                                                *
*  Copyright (c) 2025-present John Paul Valenzuela                               *
*                                                                                *
*  MIT License                                                                   *
*                                                                                *
*  Permission is hereby granted, free of charge, to any person obtaining a copy  *
*  of this software and associated documentation files (the "Software"), to      *
*  deal in the Software without restriction, including without limitation the    *
*  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or   *
*  sell copies of the Software, and to permit persons to whom the Software is    *
*  furnished to do so, subject to the following conditions:                      *
*                                                                                *
*  The above copyright notice and this permission notice shall be included in    *
*  all copies or substantial portions of the Software.                           *
*                                                                                *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      *
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL       *
*  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER    *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN     *
*  THE SOFTWARE.                                                                 *
*                                                                                * 
**********************************************************************************/

#ifndef GL41_RENDERER_H
#define GL41_RENDERER_H

#include "pxl_renderer.h"

#include "gl41_shader.h"
#include "gl41_mesh.h"

class GL41Renderer : public PXLRenderer {

private:
    
    std::unordered_map<u64_t, GL41Shader> gl41_shaders;
    std::unordered_map<u64_t, Mesh> gl41_meshes;

    u32_t bound_vao = 0;
    std::vector<u32_t> bound_textures;
    u64_t current_shader = 0;

    std::vector<struct DrawCall> draw_queue;

public:
    GL41Renderer();
    ~GL41Renderer();

    u64_t add_mesh(struct Mesh& mesh) override;
    u64_t add_shader(struct Shader& shader) override;
    void submit_draw_call(const struct DrawCall& draw_call) override;
    void draw() override;
    void cleanup() override;

private:

    void draw_mesh(const u64_t& mesh_id);
    void use_shader(const u64_t& shader_id);

};

#endif