#include "BackgroundImage.h"
#include <stb_image.h>
#include <GLFW/glfw3.h>

BackgroundImage::BackgroundImage(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath) :
    backgroundShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str()),backgroundTexture(0),VAO(0),VBO(0) {


    //vertices
    float backgroundRectangle[] = {

         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, 
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 
    
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f 
    };

    //buffers
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundRectangle), &backgroundRectangle, GL_STATIC_DRAW);
   
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    

    //texture

    glGenTextures(1, &backgroundTexture);
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

        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
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

    backgroundShader.use();
    backgroundShader.setInt("texture1", 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BackgroundImage::render()
{
    backgroundShader.use();
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);
}

BackgroundImage::~BackgroundImage()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &backgroundTexture);
}
