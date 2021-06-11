#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader/Shader.h"
#include "Framebuffer/Framebuffer.h"
#include "BoundingRegion/BoundingRegion.h"

struct DirLight
{
	/* Direction constant for all parallel rays */
	glm::vec3 direction;

	/* Light data */
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	/* Bounding region for shadows */
	BoundingRegion br;

	/* Transformation to light space (projection * view) */
	glm::mat4 lightSpaceMatrix;

	/* FBO for shadows */
	Framebuffer shadowFBO;

	/* Constructor */
	DirLight(glm::vec3 _direction, glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular, BoundingRegion _br, unsigned int _shadowRes);

	/* Update light space matrix */
	void updateMatrices();

	void render(Shader _shader, unsigned int _textureIdx);
};

#endif // !LIGHT_H