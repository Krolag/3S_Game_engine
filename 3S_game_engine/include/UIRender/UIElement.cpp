#include "UIElement.h"

namespace UIRender
{
    // Constructor with initialization list
    UIElement::UIElement(std::string vertexShaderPath, std::string fragmentShaderPath, std::string textureDirectory, std::string texturePath, float leftPos, float rightPos, float bottomPos, float topPos) :
        uiShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str()),
        uiTexture(textureDirectory, texturePath),
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
        uiVAO(0), uiVBO(0), uiEBO(0)
    {
        // generate vertex array following vertex and element buffers
        glGenVertexArrays(1, &uiVAO);
        glGenBuffers(1, &uiVBO);
        glGenBuffers(1, &uiEBO);

        glBindVertexArray(uiVAO);

        // vertex buffer data
        glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(uiVertices), &uiVertices, GL_STATIC_DRAW);

        // element buffer data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uiIndices), uiIndices, GL_STATIC_DRAW);

        // vertex array data on specific position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        // load texture
        uiTexture.load(false);
    }

    // Destructor
    UIElement::~UIElement()
    {
        glDeleteVertexArrays(1, &uiVAO);
        glDeleteBuffers(1, &uiEBO);
        glDeleteBuffers(1, &uiVBO);
    }

    // Method draws ui element in render loop
    void UIElement::render()
    {
        uiShader.use(); // we want to use shader belonging to this object

        glDisable(GL_DEPTH_TEST); // disabling depth test to make ui being on top of everything

        glBindVertexArray(uiVAO); // bind vertex array belonging to this object

        uiTexture.bind(); // bind ui texture to use it
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draw call

        glBindVertexArray(0); // unbind vertex array belonging to this object

        glEnable(GL_DEPTH_TEST); // depth test being enabled back
    }
}