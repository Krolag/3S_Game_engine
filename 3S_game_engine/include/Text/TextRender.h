#pragma once
#include <iostream>
#include <map>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader/Shader.h>

#include <ft2build.h>
#include FT_FREETYPE_H

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextRender
{
private:
    unsigned int textVBO, textVAO;
    FT_Library ft;
    FT_Face face;
    std::map <GLchar, Character> Characters;
    //float x, y, scale;
    std::string fontPath;
    Shader textShader;

public:
    TextRender(std::string vertexShaderPath, std::string fragmentShaderPath, std::string fontPath);
    
    ~TextRender();

    //1st argument is shader program which will create text, 2nd is text to render, 3rd and 4th are position in X and Y axis, 
    //5th is scale of text, 6th is vec3 of color
    void render(Shader& textShader, std::string text, float x, float y, float scale, glm::vec3 color);
};
