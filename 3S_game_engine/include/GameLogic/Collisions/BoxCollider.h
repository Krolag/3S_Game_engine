/**
 * @class	BoxCollider
 * @brief	Class responsible for AABB bounding boxes around models
 *
 * @author	Kuba Podkomórka 224407
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Primitives/Box/Box.h"
#include "Loader/Model.h"

class BoxCollider
{
private:
	glm::vec3 min, max;
	glm::mat4 transform;
	Box collisionBox;
public:
	/* Constructors and destructors */
	BoxCollider(Box _box, glm::mat4 _transform);
	BoxCollider(Shader _collisionBoxShader, Loader::Model _model, glm::mat4 _transform);
	BoxCollider(Shader _collsiionBoxShader, glm::vec3 _min, glm::vec3 _max, glm::mat4 _transform);
	~BoxCollider();
	
	/* Main class methods */
	void update();
	void render();
	void compareAndUpdateMin(glm::vec3 position);
	void compareAndUpdateMax(glm::vec3 position);

	/* Transform methods */
	void translate(glm::vec3 translate);
	void rotate(float degrees, glm::vec3 axis);
	void scale(glm::vec3 scale);
	void copyTransform(glm::mat4 sourceMatrix);
	
	/* Setters */
	void setMin(glm::vec3 _min);
	void setMax(glm::vec3 _max);

	/* Getters */
	glm::vec3 getMin();
	glm::vec3 getMax();
};