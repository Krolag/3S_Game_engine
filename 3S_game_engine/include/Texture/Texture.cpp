//#include "Texture.h"
//
//#include <stb_image.h>
//#include <iostream>
//
//Texture::Texture(const std::string& path) : ID(0), filePath(path), imageDataBuffer(nullptr), width(0), height(0), nrChannels(0)
//{
//	stbi_set_flip_vertically_on_load(true);
//
//	// generate and bind
//	glGenTextures(1, &ID);
//	glBindTexture(GL_TEXTURE_2D, ID); // ASK IF HERE SHOUDNT BE TYPE FIELD
//
//	// set texture parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// load image
//	imageDataBuffer = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
//
//	if(imageDataBuffer)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageDataBuffer);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		std::cout << "Failed o load texture" << std::endl;
//	}
//
//	// unbind and free image local buffer
//	glBindTexture(GL_TEXTURE_2D, 0);
//	if (imageDataBuffer) stbi_image_free(imageDataBuffer);
//}
//
//Texture::~Texture()
//{
//	glDeleteTextures(1, &ID);
//}
//
//void Texture::Bind() const
//{
//	glBindTexture(GL_TEXTURE_2D, ID);
//}
//
//void Texture::Unbind()
//{
//	glBindTexture(GL_TEXTURE_2D, 0);
//}
//
//void Texture::Active(unsigned int slot) const
//{
//	glActiveTexture(GL_TEXTURE0 + slot);
//}
//
//unsigned int Texture::GetID()
//{
//	return ID;
//}
//
//std::string Texture::GetType()
//{
//	return type;
//}
//
//void Texture::SetID(unsigned int id)
//{
//	this->ID = id;
//}
//
//void Texture::SetType(std::string _type)
//{
//	this->type = _type;
//}
//
//std::string Texture::GetPath()
//{
//	return filePath;
//}
