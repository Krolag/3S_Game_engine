#pragma once
#include <glad/glad.h>
#include <iostream>

class Framebuffer
{
public:

	Framebuffer(int screen_width, int screen_height);
	void bindFramebuffer();
	void unbindFramebuffer();
	int getTexture();

private:
	const int SCREEN_HEIGHT, SCREEN_WIDTH;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int RBO;
};

