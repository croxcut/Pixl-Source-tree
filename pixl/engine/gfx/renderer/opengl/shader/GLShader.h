#ifndef __SHADER__
#define __SHADER__

#include "../../../../util/file/LoadShader.h"
#include "../../../../util/Util.h"


class GLShader{

private:
    u32 vertexShader, fragmentShader;
    u32 programID;

public:
    GLShader(const char* vertexShaderPath, const char* fragmentShaderPath);

    void use();

    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;

private:

    // Shader() = default;

    void compile(u32 *shader);
    void createShader(const char* shaderSource, u32 *shader, GLuint shaderType);
    void createProgram();

};

#endif