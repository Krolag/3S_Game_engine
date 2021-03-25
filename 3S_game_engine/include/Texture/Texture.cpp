#include "Texture.h"

Texture::Texture(const std::string& path) : filePath(path), localBuffer(nullptr), width(0), height(0)
{
	// prob flip vertically

	// glGenTextures, glBindTextures

	// set parameters like min/mag filter and wrapping

	// Unbind

	// free image stbi for local buffer
}

Texture::~Texture()
{
	// glDeleteTextures();
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	// glBindTexture(GL_TEXTURE_2D, );
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
