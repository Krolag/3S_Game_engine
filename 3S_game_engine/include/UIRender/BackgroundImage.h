#pragma once
#include "Shader/Shader.h"
#include <iostream>

namespace UIRender
{
	class BackgroundImage
	{
	public:
		BackgroundImage(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath);
		~BackgroundImage();
		void render();

	private:
		Shader backgroundShader;
		unsigned int backgroundTexture;
		unsigned int VAO, VBO;
	};
}