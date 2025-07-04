#include "Shader.h"



static u32 _compile(const char* path, GLenum type) {

    FILE* file = fopen(path, "r");
    if(!file) {
        perror("ShaderLoader");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(length + 1);
    if(!buffer) {
        perror("Shader Mem Alloc");
        fclose(file);
        return 0;
    }

    size_t str_size = fread(buffer, 1, length, file);
    buffer[str_size] = '\0';

    fclose(file);
    
    u32 handle = glCreateShader(type);
    glShaderSource(handle, 1, &buffer, NULL);
    glCompileShader(handle);
    
    free(buffer);
    
    return handle;
}

struct Shader shader_create(const char* vs_path, const char* fs_path) {
    struct Shader self;

    self.vs_handle = _compile(vs_path, GL_VERTEX_SHADER);
    self.fs_handle = _compile(fs_path, GL_FRAGMENT_SHADER);
    self.handle = glCreateProgram();

    glAttachShader(self.handle, self.vs_handle);
    glAttachShader(self.handle, self.fs_handle);
    
    glLinkProgram(self.handle);
    
    return self;
}

void shader_delete(struct Shader self) {
    glDeleteProgram(self.handle);
    glDeleteShader(self.vs_handle);    
    glDeleteShader(self.fs_handle);    
}

void shader_use(struct Shader self) {
    glUseProgram(self.handle);
}