#pragma once
#include "Shader/Shader.h"

class UIElement
{
private:
	Shader uiShader;
	unsigned int uiVAO, uiVBO, uiEBO;
	float uiVertices[20];
	int uiIndices[6];
	unsigned int uiTexture;

public:
	UIElement(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath, float leftPos, float rightPos, float bottomPos, float topPos);
	~UIElement();
	void render();
};