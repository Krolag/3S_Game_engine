#include "BoxCollider.h"

namespace GameLogic
{
	BoxCollider::BoxCollider(ComponentType _type, Loader::Model* _model, Proctor* _proctor, Shader* _shader, bool _isStatic,
		glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
		: Component(_type, _proctor), model(_model), colliderShader(_shader), isStatic(_isStatic), isUpdated(false), center(glm::vec3(0.0f)), radius(glm::vec3(0.0f)),
		colliderVAO(0), colliderVBO(0), colliderVertices{0, 0, 0}
	{
		if (model != NULL)
		{
			init();
		}
		if (proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}
	
	void BoxCollider::init()
	{
		std::cout << "=============================\nINITIALIZING BOX COLLIDER FOR:\n" << proctor->name << std::endl;
		glm::vec3 min(FLT_MAX), max(FLT_MIN);
		
		/* Iterate through all model's vertices and find max values on each axis */
		for (unsigned int i = 0; i < model->getMeshes().size(); i++)
		{
			for (unsigned int j = 0; j < model->getMeshes().at(i).vertices.size(); j++)
			{
				extremePointsAlongDirection(model->getMeshes().at(i).vertices.at(j).position, min, max);
			}
		}

		/* Transform the min and max vectors with the model transform */
		glm::mat4 modelTrans = model->getModelMatrix();
		min = modelTrans * glm::vec4(min.x, min.y, min.z, 1.0f);
		max = modelTrans * glm::vec4(max.x, max.y, max.z, 1.0f);

		/* Calculate center point of the Bounding Box */
		center.x = (min.x + max.x) / 2.0f;
		center.y = (min.y + max.y) / 2.0f;
		center.z = (min.z + max.z) / 2.0f;

		std::cout << "---------------------------------\ncenter point after initialization:\n" << center.x << " " << center.y << " " << center.z << std::endl;
		
		/* Calculate radius of the Bounding Box */
		radius.x = glm::abs(min.x - center.x);
		radius.y = glm::abs(min.y - center.y);
		radius.z = glm::abs(min.z - center.z);

		std::cout << "---------------------------\nradius after initialization:\n" << radius.x << " " << radius.y << " " << radius.z << std::endl;

		colliderVertices[0] = center.x;
		colliderVertices[1] = center.y;
		colliderVertices[2] = center.z;

		/* Generate vertex array and buffer for collider box */
		glGenVertexArrays(1, &colliderVAO);
		glGenBuffers(1, &colliderVBO);

		/* Bind appropriate vertex array and buffer */
		glBindVertexArray(colliderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, colliderVBO);

		/* Init buffer with data */
		glBufferData(GL_ARRAY_BUFFER, sizeof(colliderVertices), colliderVertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		/* Unbind vertex array and buffer */
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	
	void BoxCollider::extremePointsAlongDirection(glm::vec3 point, glm::vec3& min, glm::vec3& max)
	{
		/* Check min */
		min.x = (point.x < min.x) ? point.x : min.x;
		min.y = (point.y < min.y) ? point.y : min.y;
		min.z = (point.z < min.z) ? point.z : min.z;

		/* Check max */
		max.x = (point.x > max.x) ? point.x : max.x;
		max.y = (point.y > max.y) ? point.y : max.y;
		max.z = (point.z > max.z) ? point.z : max.z;
	}
	
	void BoxCollider::setModel(Loader::Model* _model)
	{
		model = _model;
		init();
	}

	void BoxCollider::update()
	{
		if(model == NULL)
		{
			return;
		}
		
		/* Transform the bounding box to the same position as model */
		std::vector<glm::vec3> vertices;

		if (!isStatic || !isUpdated)
		{
			
			//glm::vec3 updateMin = glm::vec3(model->position);
			//glm::vec3 updateMax = glm::vec3();
			//
			//for (unsigned int i = 0; i < vertices.size(); i++)
			//{
			//	/* Update vertices */
			//	vertices.at(i) = getModelMatrix() * glm::vec4(vertices.at(i).x, vertices.at(i).y, vertices.at(i).z, 1.0f);
			//	
			//	/* Check min */
			//	if (updateMin.x > vertices.at(i).x) updateMin.x = vertices.at(i).x;
			//	if (updateMin.y > vertices.at(i).y) updateMin.y = vertices.at(i).y;
			//	if (updateMin.z > vertices.at(i).z) updateMin.z = vertices.at(i).z;
			//	
			//	/* Check max */
			//	if (updateMax.x < vertices.at(i).x) updateMax.x = vertices.at(i).x;
			//	if (updateMax.y < vertices.at(i).y) updateMax.y = vertices.at(i).y;
			//	if (updateMax.z < vertices.at(i).z) updateMax.z = vertices.at(i).z;
			//}

			//radius.x = glm::abs(updateMax.x - updateMin.x) / 2.0f;
			//radius.y = glm::abs(updateMax.y - updateMin.y) / 2.0f;
			//radius.z = glm::abs(updateMax.z - updateMin.z) / 2.0f;
		}
		
		if(!isUpdated)
		{
			isUpdated = true;
		}
	}

	void BoxCollider::render()
	{
		//std::cout << "====================\nBOX COLLIDER RENDER:\n" << std::endl;

		/* Update vertex data */
		colliderVertices[0] = center.x;
		colliderVertices[1] = center.y;
		colliderVertices[2] = center.z;

		//std::cout << "----------\nvert data:\n" << colliderVertices[0] << " " << colliderVertices[1] << " " << colliderVertices[2] << std::endl;
		
		/* Use collider shader and bind appropriate vertex array and buffer */
		colliderShader->use();
		colliderShader->setUniform("radius", radius);
		colliderShader->setUniform("model", model->getModelMatrix());
		
		glBindVertexArray(colliderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, colliderVBO);
		
		/* Update buffer data */
		glBufferData(GL_ARRAY_BUFFER, sizeof(colliderVertices), colliderVertices, GL_DYNAMIC_DRAW);

		/* Draw call */
		glDrawArrays(GL_POINTS, 0, 1);
		
		/* Unbind vertex array and buffer */
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glm::vec3 BoxCollider::getRadius()
	{
		return radius;
	}

	glm::mat4 BoxCollider::getTranslateMatrix()
	{
		glm::mat4 translate = glm::mat4(1.0f);
		translate = glm::translate(translate, proctor->transform.position);
		return translate;
	}

	glm::mat4 BoxCollider::getScaleMatrix()
	{
		glm::mat4 scale = glm::mat4(1.0f);
		scale = glm::scale(scale, proctor->transform.scale);
		return scale;
	}

	/* Returns appropriate model matrix based on the transform variables */
	glm::mat4 BoxCollider::getModelMatrix() const
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, proctor->transform.position);
		model = glm::rotate(model, proctor->transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, proctor->transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, proctor->transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, proctor->transform.scale);
		return model;
	}
}