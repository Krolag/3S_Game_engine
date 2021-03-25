#pragma once
#include <stb_image.h>
#include <glad/glad.h>

#include <string>

class Texture
{
private:
	std::string filePath;
	unsigned char* localBuffer;
	int width;
	int height; // prob must add 1-2 fields like renderer ID and
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int  slot = 0) const; // Max possible slots are from 0 to 31
	void Unbind();
};