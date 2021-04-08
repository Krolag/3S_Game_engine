#include "Skybox/Skybox.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>

Skybox::Skybox(glm::mat4* view, glm::mat4* projection, Camera* camera) : skyboxShader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag"), view(view), projection(projection), camera(camera)
{
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // initialize skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // initialize skybox texture paths array
    skyboxTexturesPaths[0] = "assets/textures/skybox/right.jpg";
    skyboxTexturesPaths[1] = "assets/textures/skybox/left.jpg";
    skyboxTexturesPaths[2] = "assets/textures/skybox/top.jpg";
    skyboxTexturesPaths[3] = "assets/textures/skybox/bottom.jpg";
    skyboxTexturesPaths[4] = "assets/textures/skybox/front.jpg";
    skyboxTexturesPaths[5] = "assets/textures/skybox/back.jpg";

    // load cube map for skybox
    glGenTextures(1, &skyboxTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char* data = stbi_load(skyboxTexturesPaths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << skyboxTexturesPaths[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    skyboxShader.use();
    skyboxShader.setUniform("skybox", 0);
}

Skybox::~Skybox()
{
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);
}

void Skybox::render()
{
    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    skyboxShader.use();
    *view = glm::mat4(glm::mat3((*camera).GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setUniform("view", *view);
    skyboxShader.setUniform("projection", *projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}