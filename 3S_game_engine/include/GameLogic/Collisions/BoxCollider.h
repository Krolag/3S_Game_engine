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
	/* Variables to draw */
	Shader colliderShader;
	unsigned int colliderVBO, colliderVAO;
	float centerPoint[3];

	/* Min and max x, y, z coordinates */
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 radius;
public:
	/* Box collider transformation */
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	/* Constructors and destructors */
	BoxCollider(Shader _colliderShader, Loader::Model* _model, glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale);
	//BoxCollider(Shader _collsiionBoxShader, glm::vec3 _min, glm::vec3 _max, glm::vec3 _position, glm::vec3 _scale);
	~BoxCollider();

	/* Main class methods */
	void update();
	void render();
	void compareAndUpdateMin(glm::vec3 position);
	void compareAndUpdateMax(glm::vec3 position);
	void calcCenter();
	void calcRadius();

	/* Getters */
	std::vector<glm::vec3> getColliderBoxVertices();
	glm::vec3 getMin() const;
	glm::vec3 getMax() const;
	glm::vec3 getRadius() const;
	glm::vec3 getCenter() const;
	glm::mat4 getModelMatrix() const;
	glm::mat4 getTranslateMatrix() const;
	glm::mat4 getRotateMatrix() const;
	glm::mat4 getScaleMatrix() const;

	/* Setters */
	void setMin(glm::vec3 _min);
	void setMax(glm::vec3 _max);
	void setCenter(glm::vec3 _center);
	void setPosition(glm::vec3 _position);
	void setScale(glm::vec3 _scale);
	void setRotation(glm::quat _rotation);
};