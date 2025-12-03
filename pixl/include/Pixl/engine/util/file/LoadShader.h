#ifndef __LOAD_SHADER__
#define __LOAD_SHADER__

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "../Log.h"

inline const char* loadShaderFromFile(const char* shaderFile) {
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