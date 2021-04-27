#include "BoxCollider.h"

namespace GameLogic
{
	BoxCollider::BoxCollider(ComponentType _type, Loader::Model* _model, Proctor* _proctor, Shader* _shader, bool _isStatic,
		glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
		: Component(_type, _proctor), model(_model), colliderShader(_shader), isStatic(_isStatic), isUpdated(false)
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
		/* Iterate through all model's vertices and find max values on each axis */
		for (unsigned int i = 0; i < model->getMeshes().size(); i++)
		{
			for (unsigned int j = 0; j < model->getMeshes().at(i).vertices.size(); j++)
			{
				glm::vec3 tmp = model->getMeshes().at(i).vertices.at(j).position;
				compareAndUpdateMin(tmp);
				compareAndUpdateMax(tmp);
			}
		}

		/* Transform the min and max vectors with the model transform */
		glm::mat4 modelTrans = model->getModelMatrix();
		min = modelTrans * glm::vec4(min.x, min.y, min.z, 1.0f);
		max = modelTrans * glm::vec4(max.x, max.y, max.z, 1.0f);

		/* Calculate center point of the cube */
		calcCenter();
		calcRadius();

		/* Initialize gl objects for correct rendering collider box */
		glGenBuffers(1, &colliderVBO);
		glGenVertexArrays(1, &colliderVAO);

		glBindVertexArray(colliderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, colliderVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(centerPoint), &centerPoint, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

		glBindVertexArray(0);
	}

	void BoxCollider::setModel(Loader::Model* _model)
	{
		model = _model;
		init();
	}

	/* Method will render bounding box around the model if necessary */
	void BoxCollider::render()
	{
		// Render it using gl lines with some color and transform with appropriate model matrix
		colliderShader->setUniform("model", model->getModelMatrix());
		colliderShader->setUniform("radius", radius);
		colliderShader->setUniformBool("collision", false);
		glBindVertexArray(colliderVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}

	void BoxCollider::update()
	{
		if(model == NULL)
		{
			return;
		}
		
		/* Transform the bounding box to the same position as model */
		std::vector<glm::vec3> vertices = getColliderBoxVertices();

		if (!isStatic || !isUpdated)
		{
			glm::vec3 updateMin = glm::vec3(proctor->transform.position);
			glm::vec3 updateMax = glm::vec3(proctor->transform.position);
			
			for (unsigned int i = 0; i < vertices.size(); i++)
			{
				/* Update vertices */
				vertices.at(i) = getModelMatrix() * glm::vec4(vertices.at(i).x, vertices.at(i).y, vertices.at(i).z, 1.0f);
				
				/* Check min */
				if (updateMin.x > vertices.at(i).x) updateMin.x = vertices.at(i).x;
				if (updateMin.y > vertices.at(i).y) updateMin.y = vertices.at(i).y;
				if (updateMin.z > vertices.at(i).z) updateMin.z = vertices.at(i).z;
				
				/* Check max */
				if (updateMax.x < vertices.at(i).x) updateMax.x = vertices.at(i).x;
				if (updateMax.y < vertices.at(i).y) updateMax.y = vertices.at(i).y;
				if (updateMax.z < vertices.at(i).z) updateMax.z = vertices.at(i).z;
			}

			radius.x = glm::abs(updateMax.x - updateMin.x) / 2.0f;
			radius.y = glm::abs(updateMax.y - updateMin.y) / 2.0f;
			radius.z = glm::abs(updateMax.z - updateMin.z) / 2.0f;
		}
		
		if(!isUpdated)
		{
			isUpdated = true;
		}
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

	/* Method will compare given vecter and update minimum position */
	void BoxCollider::compareAndUpdateMin(glm::vec3 _value)
	{
		if (min.x > _value.x)	min.x = _value.x;
		if (min.y > _value.y)	min.y = _value.y;
		if (min.z > _value.z)	min.z = _value.z;
	}

	/* Method will compare given vecter and update maximum position */
	void BoxCollider::compareAndUpdateMax(glm::vec3 _value)
	{
		if (max.x < _value.x)	max.x = _value.x;
		if (max.y < _value.y)	max.y = _value.y;
		if (max.z < _value.z)	max.z = _value.z;
	}

	/* Method calculates center based on min and max vectors */
	void BoxCollider::calcCenter()
	{
		centerPoint[0] = (min.x + max.x) / 2.0f;
		centerPoint[1] = (min.y + max.y) / 2.0f;
		centerPoint[2] = (min.y + max.y) / 2.0f;
	}

	void BoxCollider::calcRadius()
	{
		radius.x = glm::abs(centerPoint[0] - min.x);
		radius.y = glm::abs(centerPoint[1] - min.y);
		radius.z = glm::abs(centerPoint[2] - min.z);
	}

	std::vector<glm::vec3> BoxCollider::getColliderBoxVertices()
	{
		std::vector<glm::vec3> verts;
		// FRONT FACE
		verts.push_back(glm::vec3(min.x, max.y, min.z));
		verts.push_back(glm::vec3(max.x, max.y, min.z));
		verts.push_back(glm::vec3(max.x, min.y, min.z));
		verts.push_back(glm::vec3(min.x, min.y, min.z));
		// BACK FACE
		verts.push_back(glm::vec3(min.x, max.y, max.z));
		verts.push_back(glm::vec3(max.x, max.y, max.z));
		verts.push_back(glm::vec3(max.x, min.y, max.z));
		verts.push_back(glm::vec3(min.x, min.y, max.z));
		return verts;
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