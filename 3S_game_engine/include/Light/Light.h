#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "Shader/Shader.h"

struct PointLight
{
	/* Light data */
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	/* Attenuation constants */
	float k0;
	float k1;
	float k2;

	void render(Shader _shader, int _idx);
};

struct DirLight
{
	/* Light data */
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void render(Shader _shader);
};

struct SpotLight
{
	/* Light data */
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	/* Attenuation constants */
	float k0;
	float k1;
	float k2;

	void render(Shader _shader, int _idx);
};

#endif // !LIGHT_H
