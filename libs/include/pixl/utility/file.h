#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "log.h"

inline const char* load_shader_file(const char* shaderFile) {
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

#endif