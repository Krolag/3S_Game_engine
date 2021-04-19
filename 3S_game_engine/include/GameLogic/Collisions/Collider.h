/**
 * @class	Collider
 * @brief	Base class for various types of colliders
 *
 * @author	Kuba Podkomórka 224407
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class Collider
{
private:
	bool isStatic;
	glm::mat4* transform;

public:
	/* Constructors */
	Collider(bool _isStatic, glm::mat4* _transform);
};