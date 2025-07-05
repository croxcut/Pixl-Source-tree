#ifndef SHADER_H
#define SHADER_H

#include "../util/Types.h"
#include "../util/Util.h"

struct Shader{
    u32 vs_handle, fs_handle, handle;
};

struct Shader shader_create(const char* vs_path, const char* fs_path);
void shader_delete(struct Shader* self);
void shader_use(struct Shader* self);
void shader_uniform_setBool(struct Shader* self, const char* name, bool value);
void shader_uniform_setInt(struct Shader* self, const char* name, int value);
void shader_uniform_setFloat(struct Shader* self, const char* name, float value);

#endif