#include "Texture.h"
#include <iostream>
#include <stb_image.h>

Texture::Texture()
{
	generate();
}

Texture::Texture(std::string _directory, std::string _path, aiTextureType _type)
	: directory(_directory), path(_path), type(_type) 
{
	generate();
}

Texture::Texture(std::string _directory, std::string _path)
	: directory(_directory), path(_path), type(aiTextureType_NONE)
{
	generate();
}

Texture::Texture(std::string _name)
	: name(_name), type(aiTextureType_NONE)
{
	generate();
}

void Texture::generate()
{
	glGenTextures(1, &id);
}

void Texture::load(bool _flip)
{
	stbi_set_flip_vertically_on_load(_flip);

	/* Load data texture */
	int width, height, nrChannels;
	unsigned char* data = stbi_load((directory + "/" + path).c_str(), &width, &height, &nrChannels, 0);

	/* Check number of channels and assign correct value */
	GLenum colorMode = GL_RGB; // the most used value
	switch (nrChannels) 
	{
	case 1:
		colorMode = GL_RED;
		break;
	case 4:
		colorMode = GL_RGBA;
		break;
	}

	/* Check if data loaded properly */
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		//std::cerr << "Failed to load image at " << path << std::endl;
	}

	stbi_image_free(data);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::allocate(GLenum _format, GLuint _width, GLuint _height, GLenum _type)
{
	glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _format, _type, NULL);
}

void Texture::cleanup()
{
	glDeleteTextures(1, &id);
}

void Texture::setParams(
	bool _texFilterSet,
	GLenum _texMinFilter,
	GLenum _texMagFilter,
	bool _wrapSet,
	GLenum _wrapS,
	GLenum _wrapT)
{
	if (_texFilterSet)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _texMinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _texMagFilter);
	}
	if (_wrapSet)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapT);
	}
}
