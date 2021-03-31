#pragma once
#include "Shader/Shader.h"
#include <iostream>
class BackgroundImage
{
private:

	Shader backgroundShader;

	unsigned int backgroundTexture;
	unsigned int VAO, VBO;


public:

	BackgroundImage(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath);

	void render();

	~BackgroundImage();

};

