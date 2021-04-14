#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include <glm/glm.hpp>

struct Material
{
	/* Lighting values */
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	static Material emerald;
	static Material greenRubber;

	/* Function to mix two materials with a proportion */
	static Material mix(Material _m1, Material _m2, float _mix = 0.5f);
}; 

#endif // !MATERIAL_H