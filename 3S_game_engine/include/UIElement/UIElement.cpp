#include "UIElement.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>

UIElement::UIElement(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath, float leftPos, float rightPos, float bottomPos, float topPos) :
    uiShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str()),
    uiVertices{
    //      x                       y            z    // texture coords
     -1.f + 2 * leftPos,     -1.f + 2 * topPos, 0.0f,  0.0f,  1.0f,  // left top
    -1.f + 2 * rightPos,     -1.f + 2 * topPos, 0.0f,  1.0f,  1.0f,  // right top
    -1.f + 2 * rightPos,  -1.f + 2 * bottomPos, 0.0f,  1.0f,  0.0f,  // right bottom
     -1.f + 2 * leftPos,  -1.f + 2 * bottomPos, 0.0f,  0.0f,  0.0f   // left bottom
},
uiIndices{
    0, 1, 2,
    2, 3, 0
},
uiVAO(0), uiVBO(0), uiEBO(0), uiTexture(0)
{
    // gen array and buffers
    glGenVertexArrays(1, &uiVAO);
    glGenBuffers(1, &uiVBO);
    glGenBuffers(1, &uiEBO);

    glBindVertexArray(uiVAO);

    // Vertex buffer data
    glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uiVertices), &uiVertices, GL_STATIC_DRAW);

    // element buffer data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uiIndices), uiIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Generate texture
    glGenTextures(1, &uiTexture);

    int width, height, nrComponents;

    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, uiTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << texturePath.c_str() << std::endl;
        stbi_image_free(data);
    }

    uiShader.use();
    uiShader.setUniform("texture1", 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

UIElement::~UIElement()
{
    glDeleteVertexArrays(1, &uiVAO);
    glDeleteBuffers(1, &uiEBO);
    glDeleteBuffers(1, &uiVBO);
    glDeleteTextures(1, &uiTexture);
}

void UIElement::render()
{
    uiShader.use();
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(uiVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, uiTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
}