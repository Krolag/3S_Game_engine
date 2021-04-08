#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    const char *vertexPath, *fragmentPath;

    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    void use();

    void setUniform(const std::string& name, bool value) const;

    void setUniform(const std::string& name, int value) const;

    void setUniform(const std::string& name, float value) const;

    void setUniform(const std::string& name, const glm::vec2& value) const;

    void setUniform(const std::string& name, float x, float y) const;

    void setUniform(const std::string& name, const glm::vec3& value) const;

    void setUniform(const std::string& name, float x, float y, float z) const;

    void setUniform(const std::string& name, const glm::vec4& value) const;

    void setUniform(const std::string& name, float x, float y, float z, float w);

    void setUniform(const std::string& name, const glm::mat2& mat) const;

    void setUniform(const std::string& name, const glm::mat3& mat) const;

    void setUniform(const std::string& name, const glm::mat4& mat) const;


private:
    void checkCompileErrors(GLuint shader, std::string type);
};
#endif