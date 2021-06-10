#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader/Shader.h"
#include "Framebuffer/Framebuffer.h"
#include "BoundingRegion/BoundingRegion.h"

struct PointLight
{
	/* Light data */
	glm::vec3 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	/* Attenuation constants */
	float k0;
	float k1;
	float k2;

	void render(Shader _shader, int _idx);
};

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
	DirLight(glm::vec3 _direction, glm::vec4 _ambient, glm::vec4 _diffuse, glm::vec4 _specular, BoundingRegion _br);

	/* Update light space matrix */
	void updateMatrices();

	void render(Shader _shader, unsigned int _textureIdx);
};

struct SpotLight
{
	/* Light data */
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	/* Attenuation constants */
	float k0;
	float k1;
	float k2;

	void render(Shader _shader, int _idx);
};

#endif // !LIGHT_H