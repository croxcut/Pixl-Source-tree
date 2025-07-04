#ifndef SHADER_H
#define SHADER_H

#include "../util/Types.h"
#include "../util/Util.h"

struct Shader{
    u32 vs_handle, fs_handle, handle;
};

struct Shader shader_create(const char* vs_path, const char* fs_path);
void shader_delete(struct Shader self);
void shader_use(struct Shader self);

#endif