#include "BoxCollider.h"

namespace GameLogic
{	
	BoxCollider::BoxCollider(ComponentType _type, Loader::Model* _model, Proctor* _proctor, Shader* _shader, bool _isStatic,
		glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
		: Component(_type, _proctor), model(_model), colliderShader(_shader), isStatic(_isStatic), isUpdated(false), isColliding(false),
		center(glm::vec3(0.0f)), radius(glm::vec3(0.0f)), orientationMatrix(glm::mat4(1.0f)),
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

		/* Calculate center point of the Bounding Box */
		center.x = (min.x + max.x) / 2.0f;
		center.y = (min.y + max.y) / 2.0f;
		center.z = (min.z + max.z) / 2.0f;
		
		/* Calculate radius of the Bounding Box */
		radius.x = glm::abs(min.x - center.x);
		radius.y = glm::abs(min.y - center.y);
		radius.z = glm::abs(min.z - center.z);

		initVerticesData();

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

		model->position = proctor->transform.position;
		model->rotation = proctor->transform.rotation;
		model->scale = proctor->transform.scale;

		/* Transform the min and max vectors with the model transform */
		orientationMatrix = model->getModelMatrix();
		min = orientationMatrix * glm::vec4(min.x, min.y, min.z, 1.0f);
		max = orientationMatrix * glm::vec4(max.x, max.y, max.z, 1.0f);
		
		std::cout << "--------------------------------\nmin and max after initialization\nmin: " << min.x << " " << min.y << " " << min.z <<
			"\nmax: " << max.x << " " << max.y << " " << max.z << std::endl;

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
	}

	void BoxCollider::setModel(Loader::Model* _model)
	{
		model = _model;
		init();
	}

	bool BoxCollider::checkCollisionOBB(BoxCollider* otherCollider)
	{
		/* Get colliders vertices to check collisions on them */
		std::vector<glm::vec3> thisVertices = this->getColliderVertices();
		std::vector<glm::vec3> otherVertices = otherCollider->getColliderVertices();
		
		/* Get all 15 axis (6 from normals, 9 from finding cross products of normals A and B) to check collisions on it from both colliders */
		std::vector<glm::vec3> axes;
		glm::vec3 axis;
		glm::vec3 veca, vecb;
		
		/* Get normals of this collider faces */
		veca = thisVertices.at(4) - thisVertices.at(2);
		vecb = thisVertices.at(3) - thisVertices.at(2);
		axis = glm::normalize(glm::cross(veca, vecb));
		axes.push_back(axis);

		veca = thisVertices.at(1) - thisVertices.at(3);
		vecb = thisVertices.at(5) - thisVertices.at(3);
		axis = glm::normalize(glm::cross(veca, vecb));
		axes.push_back(axis);

		veca = thisVertices.at(2) - thisVertices.at(0);
		vecb = thisVertices.at(1) - thisVertices.at(0);
		axis = glm::normalize(glm::cross(veca, vecb));
		axes.push_back(axis);

		/* Get normals of other collider faces */
		veca = otherVertices.at(4) - otherVertices.at(2);
		vecb = otherVertices.at(3) - otherVertices.at(2);
		axis = glm::normalize(glm::cross(veca, vecb));
		axes.push_back(axis);

		veca = otherVertices.at(1) - otherVertices.at(3);
		vecb = otherVertices.at(5) - otherVertices.at(3);
		axis = glm::normalize(glm::cross(veca, vecb));
		axes.push_back(axis);

		veca = otherVertices.at(2) - otherVertices.at(0);
		vecb = otherVertices.at(1) - otherVertices.at(0);
		axis = glm::normalize(glm::cross(veca, vecb));
		axes.push_back(axis);

		/* Get other 9 axes by using cross product on calculated axes */
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 3; j < 6; ++j)
			{
				axis = glm::cross(axes.at(i), axes.at(j));
				if (glm::abs(glm::dot(axis, glm::vec3(1.0f))) > 0.0001)
					axes.push_back(axis);
			}
		}

		glm::vec3 separationVector;
		float shortestOverlap = FLT_MAX;
		
		/* Iterate through all axes to detect collision by SAT and to find the shortest overlap */
		for (int i = 0; i < axes.size(); ++i)
		{
			/* Project the points on the axis and get min and max from both colliders by using dot product */
			float thisMin = FLT_MAX;
			float thisMax = -FLT_MAX;
			float otherMIn = FLT_MAX;
			float otherMax = -FLT_MAX;
			
			/* Iterate through all this and other vertices */
			for (int j = 0; j < 8; ++j)
			{
				/* Calculate current projection for this collider */
				float currentProjection = glm::dot(thisVertices.at(j), axes.at(i));

				/* Check for min and max values for this collider */
				if (currentProjection < thisMin)
					thisMin = currentProjection;
				if (currentProjection > thisMax)
					thisMax = currentProjection;
				
				/* Calculate current projection for other collider */
				currentProjection = glm::dot(otherVertices.at(j), axes.at(i));

				/* Check for min and max values for other collider */
				if (currentProjection < otherMIn)
					otherMIn = currentProjection;
				if (currentProjection > otherMax)
					otherMax = currentProjection;
			}
			
			float overlap = lineOverlap(thisMin, thisMax, otherMIn, otherMax);
			
			if(overlap == 0.0f)
			{
				/* If there is no overlap on even one axis, there is no collision */
				isColliding = false;
				return false;
			}
			
			/* We want to keep track of the smallest overlap to be used for collision resolution */
			if (i == 0)
			{
				shortestOverlap = overlap;
				separationVector = axes.at(i);
			}
			else if(overlap < shortestOverlap)
			{
				shortestOverlap = overlap;
				separationVector = axes.at(i);
			}
		}
		/* If there is overlap on every axis there is collision */

		glm::vec3 otherToThisVector = otherCollider->proctor->transform.position - this->proctor->transform.position;
		if(glm::dot(separationVector, otherToThisVector) >= 0)
		{
			separationVector *= -1;
		}

		separationVector *= shortestOverlap;

		this->proctor->transform.position += separationVector;
		
		isColliding = true;
		return true;
	}

	float BoxCollider::lineOverlap(float minA, float maxA, float minB, float maxB)
	{
		return glm::max(0.0f, glm::min(maxA, maxB) - glm::max(minA, minB));
	}

	float BoxCollider::shortestOverlapBetweenOrdered(float val, float lowerBound, float upperBound)
	{
		float overlapLeft = glm::abs(lowerBound - val);
		float overlapRight = glm::abs(val - upperBound);
		return overlapLeft < overlapRight ? overlapLeft : overlapRight;
	}
	
	void BoxCollider::update()
	{
		if(model == NULL)
		{
			return;
		}
		
		/* Transform the bounding box to the same position as model */
		
		if (!isStatic || !isUpdated)
		{
			orientationMatrix = model->getModelMatrix();
			center = proctor->getPosition();
			center.x = colliderVertices[3] - glm::abs(colliderVertices[0] - colliderVertices[3]) / 2.0f;
			center.y = colliderVertices[3] - glm::abs(colliderVertices[0] - colliderVertices[3]) / 2.0f;
			center.z = colliderVertices[3] - glm::abs(colliderVertices[0] - colliderVertices[3]) / 2.0f;
			glm::vec4 vec = glm::vec4(radius.x, radius.y, radius.z, 1.0f);
			vec = orientationMatrix * vec;
		}
		
		if(!isUpdated)
		{
			isUpdated = true;
		}
	}

	void BoxCollider::render()
	{
		colliderShader->use();
		colliderShader->setUniform("model", orientationMatrix);
		colliderShader->setUniformBool("collision", isColliding);

		glBindVertexArray(colliderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, colliderVBO);
		
		/* Update buffer data */
		glBufferData(GL_ARRAY_BUFFER, sizeof(colliderVertices), colliderVertices, GL_DYNAMIC_DRAW);
		
		/* Draw call with line option */
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 24);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
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

	std::vector<glm::vec3> BoxCollider::calcVertices()
	{
		std::vector<glm::vec3> vertices;

		/* Front face */
		vertices.push_back(glm::vec3(center.x - radius.x, center.y + radius.y, center.z + radius.z));	// top left
		vertices.push_back(glm::vec3(center.x + radius.x, center.y + radius.y, center.z + radius.z));	// top right
		vertices.push_back(glm::vec3(center.x + radius.x, center.y - radius.y, center.z + radius.z));	// bottom right
		vertices.push_back(glm::vec3(center.x - radius.x, center.y - radius.y, center.z + radius.z));	// bottom left

		/* Back face */
		vertices.push_back(glm::vec3(center.x - radius.x, center.y + radius.y, center.z - radius.z));	// top left
		vertices.push_back(glm::vec3(center.x + radius.x, center.y + radius.y, center.z - radius.z));	// top right
		vertices.push_back(glm::vec3(center.x + radius.x, center.y - radius.y, center.z - radius.z));	// bottom right
		vertices.push_back(glm::vec3(center.x - radius.x, center.y - radius.y, center.z - radius.z));	// bottom left

		return vertices;
	}

	glm::vec3 BoxCollider::getRadius()
	{
		return radius;
	}

	glm::vec3 BoxCollider::getCenter()
	{
		return center;
	}

	std::vector<glm::vec3> BoxCollider::getColliderVertices()
	{
		std::vector<glm::vec3> colliderVerts;
		glm::vec3 colliderVert;

		for (int i = 0; i < 12; i+=3)
		{
			if(i<8)
			{
				colliderVert.x = colliderVertices[i];
				colliderVert.y = colliderVertices[i + 1];
				colliderVert.z = colliderVertices[i + 2];
				colliderVerts.push_back(colliderVert);
				colliderVerts.back() = model->getModelMatrix() * glm::vec4(colliderVerts.back().x, colliderVerts.back().y, colliderVerts.back().z, 1.0f);
				colliderVert.z = -colliderVert.z;
				colliderVerts.push_back(colliderVert);
				colliderVerts.back() = model->getModelMatrix() * glm::vec4(colliderVerts.back().x, colliderVerts.back().y, colliderVerts.back().z, 1.0f);
			}
			else
			{
				colliderVert.x = colliderVertices[i + 3];
				colliderVert.y = colliderVertices[i + 4];
				colliderVert.z = colliderVertices[i + 5];
				colliderVerts.push_back(colliderVert);
				colliderVerts.back() = model->getModelMatrix() * glm::vec4(colliderVerts.back().x, colliderVerts.back().y, colliderVerts.back().z, 1.0f);
				colliderVert.z = -colliderVert.z;
				colliderVerts.push_back(colliderVert);
				colliderVerts.back() = model->getModelMatrix() * glm::vec4(colliderVerts.back().x, colliderVerts.back().y, colliderVerts.back().z, 1.0f);
			}
		}
		
		return colliderVerts;
	}

	glm::mat4 BoxCollider::getTranslationMatrix()
	{
		glm::mat4 translationM = glm::mat4(1.0f);
		translationM[3][0] = orientationMatrix[3][0];
		translationM[3][1] = orientationMatrix[3][1];
		translationM[3][2] = orientationMatrix[3][2];
		return translationM;
	}

	glm::mat4 BoxCollider::getRotationMatrix()
	{
		glm::mat4 rotationM = orientationMatrix;
		glm::vec3 scalingFactor = getScalingFactorFromMatrix();
		for (int i = 0; i < 3; ++i)
		{
			rotationM[3][i] = 0.0f;
			rotationM[i][3] = 0.0f;
			rotationM[0][i] *= 1/scalingFactor.x;
			rotationM[1][i] *= 1/scalingFactor.y;
			rotationM[2][i] *= 1/scalingFactor.z;
		}
		return rotationM;
	}

	glm::vec3 BoxCollider::getScalingFactorFromMatrix()
	{
		glm::vec3 scalingFactor;
		scalingFactor.x = glm::sqrt(glm::pow(orientationMatrix[0][0], 2) + glm::pow(orientationMatrix[0][1], 2) + glm::pow(orientationMatrix[0][2], 2));
		scalingFactor.y = glm::sqrt(glm::pow(orientationMatrix[1][0], 2) + glm::pow(orientationMatrix[1][1], 2) + glm::pow(orientationMatrix[1][2], 2));
		scalingFactor.z = glm::sqrt(glm::pow(orientationMatrix[2][0], 2) + glm::pow(orientationMatrix[2][1], 2) + glm::pow(orientationMatrix[2][2], 2));
		return scalingFactor;
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

	void BoxCollider::initVerticesData()
	{
		// FRONT
		
		// upper left
		colliderVertices[0] = center.x - radius.x; 
		colliderVertices[1] = center.y + radius.y;
		colliderVertices[2] = center.z + radius.z;

		// upper right
		colliderVertices[3] = center.x + radius.x;
		colliderVertices[4] = center.y + radius.y;
		colliderVertices[5] = center.z + radius.z;

		// bottom right
		colliderVertices[6] = center.x + radius.x;
		colliderVertices[7] = center.y - radius.y;
		colliderVertices[8] = center.z + radius.z;

		// bottom right
		colliderVertices[9] = center.x + radius.x;
		colliderVertices[10] = center.y - radius.y;
		colliderVertices[11] = center.z + radius.z;

		// bottom left
		colliderVertices[12] = center.x - radius.x;
		colliderVertices[13] = center.y - radius.y;
		colliderVertices[14] = center.z + radius.z;

		// upper left
		colliderVertices[15] = center.x - radius.x;
		colliderVertices[16] = center.y + radius.y;
		colliderVertices[17] = center.z + radius.z;

		// BACK

		// upper left
		colliderVertices[18] = center.x - radius.x;
		colliderVertices[19] = center.y + radius.y;
		colliderVertices[20] = center.z - radius.z;

		// upper right
		colliderVertices[21] = center.x + radius.x;
		colliderVertices[22] = center.y + radius.y;
		colliderVertices[23] = center.z - radius.z;

		// bottom right
		colliderVertices[24] = center.x + radius.x;
		colliderVertices[25] = center.y - radius.y;
		colliderVertices[26] = center.z - radius.z;

		// bottom right
		colliderVertices[27] = center.x + radius.x;
		colliderVertices[28] = center.y - radius.y;
		colliderVertices[29] = center.z - radius.z;

		// bottom left
		colliderVertices[30] = center.x - radius.x;
		colliderVertices[31] = center.y - radius.y;
		colliderVertices[32] = center.z - radius.z;

		// upper left
		colliderVertices[33] = center.x - radius.x;
		colliderVertices[34] = center.y + radius.y;
		colliderVertices[35] = center.z - radius.z;

		// TOP
		colliderVertices[36] = center.x - radius.x;
		colliderVertices[37] = center.y + radius.y;
		colliderVertices[38] = center.z + radius.z;

		colliderVertices[39] = center.x + radius.x;
		colliderVertices[40] = center.y + radius.y;
		colliderVertices[41] = center.z + radius.z;

		colliderVertices[42] = center.x + radius.x;
		colliderVertices[43] = center.y + radius.y;
		colliderVertices[44] = center.z - radius.z;

		colliderVertices[45] = center.x + radius.x;
		colliderVertices[46] = center.y + radius.y;
		colliderVertices[47] = center.z - radius.z;

		colliderVertices[48] = center.x - radius.x;
		colliderVertices[49] = center.y + radius.y;
		colliderVertices[50] = center.z - radius.z;

		colliderVertices[51] = center.x - radius.x;
		colliderVertices[52] = center.y + radius.y;
		colliderVertices[53] = center.z + radius.z;

		// BOTTOM
		colliderVertices[54] = center.x - radius.x;
		colliderVertices[55] = center.y - radius.y;
		colliderVertices[56] = center.z + radius.z;

		colliderVertices[57] = center.x + radius.x;
		colliderVertices[58] = center.y - radius.y;
		colliderVertices[59] = center.z + radius.z;

		colliderVertices[60] = center.x + radius.x;
		colliderVertices[61] = center.y - radius.y;
		colliderVertices[62] = center.z - radius.z;

		colliderVertices[63] = center.x + radius.x;
		colliderVertices[64] = center.y - radius.y;
		colliderVertices[65] = center.z - radius.z;

		colliderVertices[66] = center.x - radius.x;
		colliderVertices[67] = center.y - radius.y;
		colliderVertices[68] = center.z - radius.z;

		colliderVertices[69] = center.x - radius.x;
		colliderVertices[70] = center.y - radius.y;
		colliderVertices[71] = center.z + radius.z;
	}

}