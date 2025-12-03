#ifndef __GL_SHADER__
#define __GL_SHADER__

#include "../../../../util/file/LoadShader.h"
#include "../../../../util/Util.h"
#include <glm/glm.hpp>

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

    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;

    u32 getProgramID() { return programID; };

    void cleanup();

private:

    // Shader() = default;

    void compile(u32 *shader);
    void createShader(const char* shaderSource, u32 *shader, GLuint shaderType);
    void createProgram();

};

#endif