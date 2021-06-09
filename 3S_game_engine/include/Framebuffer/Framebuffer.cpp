#include "Framebuffer.h"

Framebuffer::Framebuffer()
	: val(0), width(0), height(0), bitCombo(0) {}

Framebuffer::Framebuffer(GLuint _width, GLuint _height, GLbitfield _bitCombo)
	: val(0), width(_width), height(_height), bitCombo(_bitCombo) {}

void Framebuffer::generate()
{
	glGenFramebuffers(1, &val);
}

void Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, val);
}

void Framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::setViewPort()
{
	glViewport(0, 0, width, height);
}

void Framebuffer::clear()
{
	glClear(bitCombo);
}

void Framebuffer::activate()
{
	bind();
	setViewPort();
	clear();
}

void Framebuffer::allocateAndAttachRBO(GLenum _attachType, GLenum _format)
{
	// GL_DEPTH_STENCIL_ATTACHMENT, GL_DEPTH24_STENCIL8
	GLuint rbo;

	/* Generate */
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	/* Attach */
	glRenderbufferStorage(GL_RENDERBUFFER, _format, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, _attachType, GL_RENDERBUFFER, rbo);

	/* Add to list */
	RBOs.push_back(rbo);
}

void Framebuffer::allocateAndAttachTexture(GLenum _attachType, GLenum _format, GLenum _type, 
	bool _texFilterSet, GLenum _texMinFilter, GLenum _texMagFilter, bool _wrapSet, GLenum _wrapS, GLenum _wrapT)
{
	std::string name = "tex" + textures.size();
	Texture tex(name);

	/* Allocate */
	tex.bind();
	tex.allocate(_format, width, height, _type);
	Texture::setParams(_texFilterSet, _texMinFilter, _texMagFilter, _wrapSet, _wrapS, _wrapT);

	/* Attach */
	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, _attachType, GL_TEXTURE_2D, tex.id, 0);

	/* Add to list */
	textures.push_back(tex);
}

void Framebuffer::attachTexture(GLenum _attachType, Texture tex)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, _attachType, GL_TEXTURE_2D, tex.id, 0);
}

void Framebuffer::cleanup()
{
	/* Delete RBOs */
	glDeleteRenderbuffers(RBOs.size(), &RBOs.at(0));

	/* Delete generated textures */
	for (Texture t : textures)
		t.cleanup();

	/* Delete FBO */
	glDeleteFramebuffers(1, &val);
}