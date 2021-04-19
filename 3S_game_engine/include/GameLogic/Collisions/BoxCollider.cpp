/**
 * @class	BoxCollider
 * @brief	Management of bounding boxes on the models
 *
 * @author	Kuba Podkomórka 224407
 */

#include "BoxCollider.h"

// CONSTRUCTORS

/* Constructor will make bounding box around given box */
BoxCollider::BoxCollider(Box _box, glm::mat4 _transform) : min(glm::vec3(-0.f, -0.f, -0.f)), max(glm::vec3(0.f, 0.f, 0.f)), transform(_transform)
{
	std::cout << "Initializing the box collider based on the box object..." << std::endl;
	copyTransform(_transform);
	float* boxVerts = _box.getBoxVertices();
	/* Iterate throughout all the box vertices and find the minimum and maximum x,y and z values*/
	for (int i = 0; i < 36; i++)
	{
		glm::vec3 vertVector(boxVerts[i * 5 + 0], boxVerts[i * 5 + 1], boxVerts[i * 5 + 2]);
		compareAndUpdateMin(vertVector);
		compareAndUpdateMax(vertVector);
	}
	
	/* Transform the min and max x,y,z vectors with the model transform matrix */
	min = transform * glm::vec4(min.x, min.y, min.z, 1.0f);
	max = transform * glm::vec4(max.x, max.y, max.z, 1.0f);

	collisionBox = Box(_box.getShader(), min, max);
	std::cout << "Box collider initialization is finished. Program found min and max positions for the collision box" << std::endl;
}

/* Constructor will make bounding box around given model */
BoxCollider::BoxCollider(Shader _collisionBoxShader, Loader::Model _model, glm::mat4 _transform) : min(glm::vec3(-1.f, -1.f, -1.f)), max(glm::vec3(1.f, 1.f, 1.f))
{
	std::cout << "Initializing the box collider based on the model... Be patient if model has many vertices and wait until finish..." << std::endl;
	copyTransform(_transform);
	/* Iterate throughout all the model vertices and find the minimum and maximum x,y and z values*/
	for (int i = 0; i < _model.getMeshes().size(); i++)
	{
		for (int j = 0; j < _model.getMeshes().at(i).vertices.size(); j++)
		{
			compareAndUpdateMin(_model.getMeshes().at(i).vertices.at(j).position);
			compareAndUpdateMax(_model.getMeshes().at(i).vertices.at(j).position);
		}
	}

	/* Transform the min and max x,y,z vectors with the model transform matrix */
	min = transform * glm::vec4(min.x, min.y, min.z, 1.0f);
	max = transform * glm::vec4(max.x, max.y, max.z, 1.0f);

	/* Generate the box, which will surround the model */
	collisionBox = Box(_collisionBoxShader, min, max);
	std::cout << "Box collider initialization is finished. Program found min and max positions for the collision box" << std::endl;
}

/* Constructor with already known min and max values */
BoxCollider::BoxCollider(Shader _collisionBoxShader, glm::vec3 _min, glm::vec3 _max, glm::mat4 _transform) : min(_min), max(_max)
{
	collisionBox = Box(_collisionBoxShader, min, max);
}

BoxCollider::~BoxCollider()
{
	
}

// MAIN CLASS METHODS

/* Method updates bounding box around model after it changes position or rotation etc. */
void BoxCollider::update()
{
	// Transform the bounding box the same as model based on model matrix
	collisionBox.setTransform(transform);
	std::cout << "Collision box transform: " << transform.operator[](0).operator[](0) << " " << transform.operator[](1).operator[](1)
		<< " " << transform.operator[](2).operator[](2) << std::endl;

	std::cout << "Box transform: " << collisionBox.getTransform().operator[](0).operator[](0) << " "
	<< collisionBox.getTransform().operator[](1).operator[](1) << " "
	<< collisionBox.getTransform().operator[](2).operator[](2) << std::endl;
	
	// Calculate new bounding box around existing one
}

/* Method will render bounding box around the model if necessary */
void BoxCollider::render()
{
	// Render it using gl lines with some color and transform with appropriate model matrix
	collisionBox.render(true);
}

/* Method will compare given vecter and update minimum position */
void BoxCollider::compareAndUpdateMin(glm::vec3 position)
{
	if (min.x > position.x) min.x = position.x;
	if (min.y > position.y) min.y = position.y;
	if (min.z > position.z) min.z = position.z;
}

/* Method will compare given vecter and update maximum position */
void BoxCollider::compareAndUpdateMax(glm::vec3 position)
{
	if (max.x < position.x) max.x = position.x;
	if (max.y < position.y) max.y = position.y;
	if (max.z < position.z) max.z = position.z;
}

// TRANSFORM METHODS

/* Translation of the collision box */
void BoxCollider::translate(glm::vec3 translate)
{
	transform = glm::translate(transform, translate);
}

/* Rotation of the collision box */
void BoxCollider::rotate(float degrees, glm::vec3 axis)
{
	collisionBox.rotate(degrees, axis);
}

/* Scale of the collision box */
void BoxCollider::scale(glm::vec3 scale)
{
	transform = glm::scale(transform, scale);
}

/* Sets the transform matrix with other matrix values */
void BoxCollider::copyTransform(glm::mat4 sourceMatrix)
{
	//transform = glm::mat4(sourceMatrix);
	transform = glm::make_mat4(glm::value_ptr(sourceMatrix));
}

// GETTERS

void BoxCollider::setMin(glm::vec3 _min)
{
	min = _min;
}

void BoxCollider::setMax(glm::vec3 _max)
{
	max = _max;
}

// SETTERS

glm::vec3 BoxCollider::getMin()
{
	return min;
}

glm::vec3 BoxCollider::getMax()
{
	return max;
}