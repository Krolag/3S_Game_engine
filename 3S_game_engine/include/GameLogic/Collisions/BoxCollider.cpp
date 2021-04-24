/**
 * @class	BoxCollider
 * @brief	Management of bounding boxes on the models
 *
 * @author	Kuba Podkomórka 224407
 */

//#include "BoxCollider.h"

// CONSTRUCTORS

/* Constructor will make bounding box around given model */
//BoxCollider::BoxCollider(Shader _colliderShader, Loader::Model* _model, glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale) :
//	colliderShader(_colliderShader), position(glm::vec3(_position)), rotation(glm::quat(_rotation)), scale(glm::vec3(_scale)), min(glm::vec3(1000.0f)), max(glm::vec3(-1000.0f))
//{
//	std::cout << "Initializing the box collider based on the model... Be patient if model has many vertices and wait until finish..." << std::endl;
//
//	/* Iterate throughout all the model vertices and find the minimum and maximum x,y and z values*/
//	for (int i = 0; i < _model->getMeshes().size(); i++)
//	{
//		std::cout << "PROCESSING MESH NUMBER " << i << ":" << std::endl;
//		for (int j = 0; j < _model->getMeshes().at(i).vertices.size(); j++)
//		{
//			std::cout << "vert number " << j << "position: "
//				<< _model->getMeshes().at(i).vertices.at(j).position.x
//				<< " " << _model->getMeshes().at(i).vertices.at(j).position.y
//				<< " " << _model->getMeshes().at(i).vertices.at(j).position.z
//				<< std::endl;
//			compareAndUpdateMin(_model->getMeshes().at(i).vertices.at(j).position);
//			compareAndUpdateMax(_model->getMeshes().at(i).vertices.at(j).position);
//		}
//		std::cout << "PROCESSING MESH NUMBER " << i << " FINISHED" << std::endl;
//	}
//	std::cout << "MODEL POSITION FROM CONSTRUCTOR COLLIDER:\nx: " << _model->position.x << ", y: " << _model->position.y << ", z: " << _model->position.z << std::endl;
//	
//	/* Transform the min and max x,y,z vectors with the model transform matrix */
//	glm::mat4 modelTransform = _model->getModelMatrix();
//	min = modelTransform * glm::vec4(min.x, min.y, min.z, 1.0f);
//	max = modelTransform * glm::vec4(max.x, max.y, max.z, 1.0f);
//
//	/* Calculate center point of the cube */
//	calcCenter();
//	calcRadius();
//
//	std::cout << "RADIUS VECTOR: " << radius.x << " " << radius.y << " " << radius.z << std::endl;
//	std::cout << "MIN VECTOR: " << min.x << " " << min.y << " " << min.z << std::endl;
//	std::cout << "MAX VECTOR: " << max.x << " " << max.y << " " << max.z << std::endl;
//	std::cout << "CENTER POINT: " << centerPoint[0] << " " << centerPoint[1] << " " << centerPoint[2] << std::endl;
//
//		
//	/* Initialize gl objects for correct rendering collider box */
//	glGenBuffers(1, &colliderVBO);
//	glGenVertexArrays(1, &colliderVAO);
//	
//	glBindVertexArray(colliderVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, colliderVBO);
//	
//	glBufferData(GL_ARRAY_BUFFER, sizeof(centerPoint), &centerPoint, GL_DYNAMIC_DRAW);
//	
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
//	
//	glBindVertexArray(0);
//}

//BoxCollider::~BoxCollider()
//{
//	
//}

// MAIN CLASS METHODS

/* Method updates bounding box around model after it changes position or rotation etc. */
//void BoxCollider::update()
//{
	// Transform the bounding box the same as model based on model matrix
	
	//std::vector<glm::vec3> vertices = getColliderBoxVertices();
	//glm::vec3 updateMin = glm::vec3(position);
	//glm::vec3 updateMax = glm::vec3(position);
	
	//for (int i = 0; i < vertices.size(); i++)
	//{
	//	// update vert
	//	vertices.at(i) = getModelMatrix() * glm::vec4(vertices.at(i).x, vertices.at(i).y, vertices.at(i).z, 1.0f);
	//	// check min
	//	if (updateMin.x > vertices.at(i).x) updateMin.x = vertices.at(i).x;
	//	if (updateMin.y > vertices.at(i).y) updateMin.y = vertices.at(i).y;
	//	if (updateMin.z > vertices.at(i).z) updateMin.z = vertices.at(i).z;
	//	// check max
	//	if (updateMax.x < vertices.at(i).x) updateMax.x = vertices.at(i).x;
	//	if (updateMax.y < vertices.at(i).y) updateMax.y = vertices.at(i).y;
	//	if (updateMax.z < vertices.at(i).z) updateMax.z = vertices.at(i).z;
	//}
	
	//radius.x = glm::abs(updateMax.x - updateMin.x) / 2.0f;
	//radius.y = glm::abs(updateMax.y - updateMin.y) / 2.0f;
	//radius.z = glm::abs(updateMax.z - updateMin.z) / 2.0f;

	// DEBUG
//	std::cout << "=============================================" << std::endl;
//	std::cout << "BOX COLLIDER CLASS VALUES AFTER UPDATE METHOD" << std::endl;
//	std::cout << "=============================================" << std::endl;
//	std::cout << "MIN VECTOR: " << updateMin.x << " " << updateMin.y << " " << updateMin.z << std::endl;
//	std::cout << "MAX VECTOR: " << updateMax.x << " " << updateMax.y << " " << updateMax.z << std::endl;
//	std::cout << "RADIUS VECTOR: " << radius.x << " " << radius.y << " " << radius.z << std::endl;
//	std::cout << "POSITION VECTOR: " << position.x << " " << position.y << " " << position.z << std::endl;
//	std::cout << "ROTATION VECTOR: " << rotation.x << " " << rotation.y << " " << rotation.z << std::endl;
//	std::cout << "SCALE VECTOR: " << scale.x << " " << scale.y << " " << scale.z << std::endl << std::endl;
//}

/* Method will render bounding box around the model if necessary */
//void BoxCollider::render()
//{
//	// Render it using gl lines with some color and transform with appropriate model matrix
//	glBindVertexArray(colliderVAO);
//	glDrawArrays(GL_POINTS, 0, 1);
//	glBindVertexArray(0);
//}


//void BoxCollider::compareAndUpdateMin(glm::vec3 _position)
//{
//	if (min.x > _position.x) min.x = _position.x;
//	if (min.y > _position.y) min.y = _position.y;
//	if (min.z > _position.z) min.z = _position.z;
//}


//void BoxCollider::compareAndUpdateMax(glm::vec3 _position)
//{
//	if (max.x < _position.x) max.x = _position.x;
//	if (max.y < _position.y) max.y = _position.y;
//	if (max.z < _position.z) max.z = _position.z;
//}


//void BoxCollider::calcCenter()
//{
//	centerPoint[0] = (min.x + max.x) / 2.0f;
//	centerPoint[1] = (min.y + max.y) / 2.0f;
//	centerPoint[2] = (min.y + max.y) / 2.0f;
//}

//void BoxCollider::calcRadius()
//{
//	radius.x = glm::abs(centerPoint[0] - min.x);
//	radius.y = glm::abs(centerPoint[1] - min.y);
//	radius.z = glm::abs(centerPoint[2] - min.z);
//}

// GETTERS

//std::vector<glm::vec3> BoxCollider::getColliderBoxVertices()
//{
//	std::vector<glm::vec3> verts;
//	// FRONT FACE
//	verts.push_back(glm::vec3(min.x, max.y, min.z));
//	verts.push_back(glm::vec3(max.x, max.y, min.z));
//	verts.push_back(glm::vec3(max.x, min.y, min.z));
//	verts.push_back(glm::vec3(min.x, min.y, min.z));
//	// BACK FACE
//	verts.push_back(glm::vec3(min.x, max.y, max.z));
//	verts.push_back(glm::vec3(max.x, max.y, max.z));
//	verts.push_back(glm::vec3(max.x, min.y, max.z));
//	verts.push_back(glm::vec3(min.x, min.y, max.z));
//	return verts;
//}

//////glm::vec3 BoxCollider::getMin() const
//////{
//////	return min;
//////}
//////
//////glm::vec3 BoxCollider::getMax() const
//////{
//////	return max;
//////}

//glm::vec3 BoxCollider::getRadius() const
//{
//	return radius;
//}

///////* Returns center of the collider box based */
//////glm::vec3 BoxCollider::getCenter() const
//////{
//////	return glm::vec3(position.x, position.y, position.z);
//////}

/* Returns appropriate model matrix based on the transform variables */
//glm::mat4 BoxCollider::getModelMatrix() const
//{
//	glm::mat4 model = glm::mat4(1.0f);
//	model = glm::translate(model, this->position);
//	model = glm::rotate(model, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//	model = glm::rotate(model, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//	model = glm::rotate(model, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
//	model = glm::scale(model, this->scale);
//	return model;
//}

//glm::mat4 BoxCollider::getTranslateMatrix() const
//{
//	glm::mat4 translate = glm::mat4(1.0f);
//	translate = glm::translate(translate, this->position);
//	return translate;
//}
//////
//////glm::mat4 BoxCollider::getRotateMatrix() const
//////{
//////	glm::mat4 rotate = glm::mat4(1.0f);
//////	rotate = glm::rotate(rotate, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
//////	rotate = glm::rotate(rotate, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
//////	rotate = glm::rotate(rotate, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
//////	return rotate;
//////}
//////
//glm::mat4 BoxCollider::getScaleMatrix() const
//{
//	glm::mat4 scale = glm::mat4(1.0f);
//	scale = glm::scale(scale, this->scale);
//	return scale;
//}
//////
//////// SETTERS
//////
//////void BoxCollider::setMin(glm::vec3 _min)
//////{
//////	min = _min;
//////}
//////
//////void BoxCollider::setMax(glm::vec3 _max)
//////{
//////	max = _max;
//////}
//////
//////void BoxCollider::setCenter(glm::vec3 _center)
//////{
//////	centerPoint[0] = _center.x;
//////	centerPoint[1] = _center.y;
//////	centerPoint[2] = _center.z;
//////}
//////
//////void BoxCollider::setPosition(glm::vec3 _position)
//////{
//////	position = _position;
//////	update();
//////}
//////
//////void BoxCollider::setScale(glm::vec3 _scale)
//////{
//////	scale = _scale;
//////	update();
//////}
//////
//////void BoxCollider::setRotation(glm::quat _rotation)
//////{
//////	rotation = _rotation;
//////	update();
//////}