#ifndef __LOAD_SHADER__
#define __LOAD_SHADER__

#include <iostream>
#include <fstream>
#include <string>

#include "Log.h"

inline std::string loadShaderFromFile(const char* shaderFile) {

    std::ifstream shader_t(shaderFile);
    
    ASSERT(shader_t, "Failed to Open Shader File at: %s", shaderFile);

    std::string content(
        (std::istreambuf_iterator<char>(shader_t)),
        std::istreambuf_iterator<char>()
    );

    return content;
}

#endif