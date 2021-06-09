#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Texture/Texture.h>
#include <iostream>
#include <vector>

class Framebuffer
{
public:
	GLuint val;
	GLuint width;
	GLuint height;
	GLbitfield bitCombo;
	std::vector<GLuint> RBOs;
	std::vector<Texture> textures;

	Framebuffer();
	Framebuffer(GLuint _width, GLuint _height, GLbitfield _bitCombo);

	void generate();
	void bind();
	void unbind();
	void setViewPort();
	void clear();
	void activate();
	void allocateAndAttachRBO(GLenum _attachType, GLenum _format);
	void allocateAndAttachTexture(
		GLenum _attachType, GLenum _format, GLenum _type,
		bool _texFilterSet = false, GLenum _texMinFilter = GL_NEAREST, GLenum _texMagFilter = GL_NEAREST, 
		bool _wrapSet = false,  GLenum _wrapS = GL_REPEAT, GLenum _wrapT = GL_REPEAT
	);
	void attachTexture(GLenum _attachType, Texture tex);
	void cleanup();
};

#endif // !FRAMEBUFFER_H