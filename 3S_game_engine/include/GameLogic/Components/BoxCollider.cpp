#include "BoxCollider.h"

namespace GameLogic
{
	BoxCollider::BoxCollider(ComponentType _type, Loader::Model* _model, Proctor* _proctor, Shader* _shader, bool _isStatic,
		glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
		: Component(_type, _proctor), model(_model), colliderShader(_shader), isStatic(_isStatic), isUpdated(false),
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
			//std::cout << "Update of radius: " << vec.x << " " << vec.y << " " << vec.z << std::endl;
			
			/* Get min and max vertices */
			
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

		///* Update vertex data */
		//colliderVertices[0] = center.x;
		//colliderVertices[1] = center.y;
		//colliderVertices[2] = center.z;

		////std::cout << "----------\nvert data:\n" << colliderVertices[0] << " " << colliderVertices[1] << " " << colliderVertices[2] << std::endl;
		//
		///* Use collider shader and bind appropriate vertex array and buffer */
		//colliderShader->use();
		//colliderShader->setUniform("radius", radius);
		//colliderShader->setUniform("model", glm::mat4(1.0f));
		//
		//glBindVertexArray(colliderVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, colliderVBO);
		//
		///* Update buffer data */
		//glBufferData(GL_ARRAY_BUFFER, sizeof(colliderVertices), colliderVertices, GL_DYNAMIC_DRAW);

		///* Draw call */
		//glDrawArrays(GL_POINTS, 0, 1);
		//
		///* Unbind vertex array and buffer */
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);

		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------

		colliderShader->use();
		colliderShader->setUniform("model", orientationMatrix /*glm::mat4(1.0f)*/);

		glBindVertexArray(colliderVAO);
		glBindBuffer(GL_ARRAY_BUFFER, colliderVBO);
		
		/* Update buffer data */
		glBufferData(GL_ARRAY_BUFFER, sizeof(colliderVertices), colliderVertices, GL_DYNAMIC_DRAW);
		
		/* Draw call */
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, 36);
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

	void BoxCollider::updatePosition(glm::vec3 oldPosition, glm::vec3 newPosition)
	{
		glm::vec3 difference = newPosition - oldPosition;
		std::cout << "\POSITION UPDATE:\nvec3(" << newPosition.x << "," << newPosition.y << "," << newPosition.z
			<< ") - vec3(" << oldPosition.x << "," << oldPosition.y << "," << oldPosition.z
			<< ") = vec3(" << difference.x << "," << difference.y << "," << difference.z << ")\n";

		std::cout << "PRINTING ORIENTATION MATRIX BEFORE TRANSLATE:\n"
			<< orientationMatrix[0][0] << " " << orientationMatrix[1][0] << " " << orientationMatrix[2][0] << " " << orientationMatrix[3][0] << "\n"
			<< orientationMatrix[0][1] << " " << orientationMatrix[1][1] << " " << orientationMatrix[2][1] << " " << orientationMatrix[3][1] << "\n"
			<< orientationMatrix[0][2] << " " << orientationMatrix[1][2] << " " << orientationMatrix[2][2] << " " << orientationMatrix[3][2] << "\n"
			<< orientationMatrix[0][3] << " " << orientationMatrix[1][3] << " " << orientationMatrix[2][3] << " " << orientationMatrix[3][3] << "\n";
		std::cout << "ONLY TRANSLATION MATRIX:\n"
			<< getTranslationMatrix()[0][0] << " " << getTranslationMatrix()[1][0] << " " << getTranslationMatrix()[2][0] << " " << getTranslationMatrix()[3][0] << "\n"
			<< getTranslationMatrix()[0][1] << " " << getTranslationMatrix()[1][1] << " " << getTranslationMatrix()[2][1] << " " << getTranslationMatrix()[3][1] << "\n"
			<< getTranslationMatrix()[0][2] << " " << getTranslationMatrix()[1][2] << " " << getTranslationMatrix()[2][2] << " " << getTranslationMatrix()[3][2] << "\n"
			<< getTranslationMatrix()[0][3] << " " << getTranslationMatrix()[1][3] << " " << getTranslationMatrix()[2][3] << " " << getTranslationMatrix()[3][3] << "\n";
		orientationMatrix = glm::translate(orientationMatrix, difference);
		std::cout << "PRINTING ORIENTATION MATRIX AFTER TRANSLATE:\n"
			<< orientationMatrix[0][0] << " " << orientationMatrix[1][0] << " " << orientationMatrix[2][0] << " " << orientationMatrix[3][0] << "\n"
			<< orientationMatrix[0][1] << " " << orientationMatrix[1][1] << " " << orientationMatrix[2][1] << " " << orientationMatrix[3][1] << "\n"
			<< orientationMatrix[0][2] << " " << orientationMatrix[1][2] << " " << orientationMatrix[2][2] << " " << orientationMatrix[3][2] << "\n"
			<< orientationMatrix[0][3] << " " << orientationMatrix[1][3] << " " << orientationMatrix[2][3] << " " << orientationMatrix[3][3] << "\n";

		std::cout << "ONLY TRANSLATION MATRIX:\n"
		<< getTranslationMatrix()[0][0] << " " << getTranslationMatrix()[1][0] << " " << getTranslationMatrix()[2][0] << " " << getTranslationMatrix()[3][0] << "\n"
		<< getTranslationMatrix()[0][1] << " " << getTranslationMatrix()[1][1] << " " << getTranslationMatrix()[2][1] << " " << getTranslationMatrix()[3][1] << "\n"
		<< getTranslationMatrix()[0][2] << " " << getTranslationMatrix()[1][2] << " " << getTranslationMatrix()[2][2] << " " << getTranslationMatrix()[3][2] << "\n"
		<< getTranslationMatrix()[0][3] << " " << getTranslationMatrix()[1][3] << " " << getTranslationMatrix()[2][3] << " " << getTranslationMatrix()[3][3] << "\n";
		glm::vec4 vec(center.x, center.y, center.z, 1.0f);
		std::cout << "\nCENTER VEC4 BEFORE MATRIX MULTIPLYING: " << vec.x << " " << vec.y << " " << vec.z << " " << vec.w << "\n";
		vec = getTranslationMatrix() * vec;
		std::cout << "\nCENTER VEC4 AFTER MATRIX MULTIPLYING: " << vec.x << " " << vec.y << " " << vec.z << " " << vec.w << "\n";

		std::cout << "\nCENTER UPDATE:\nold: " << center.x << " " << center.y << " " << center.z;
		center += difference;
		std::cout << "\nnew: " << center.x << " " << center.y << " " << center.z << "\n";
	}

	void BoxCollider::updateRotation(glm::quat oldRotation, glm::quat newRotation)
	{
		glm::vec3 difference = glm::vec3(newRotation.x, newRotation.y, newRotation.z ) - glm::vec3(oldRotation.x, oldRotation.y, oldRotation.z);
		std::cout << "\ROTATION UPDATE:\nvec3(" << newRotation.x << "," << newRotation.y << "," << newRotation.z
			<< ") - vec3(" << oldRotation.x << "," << oldRotation.y << "," << oldRotation.z
			<< ") = vec3(" << difference.x << "," << difference.y << "," << difference.z << ")\n";
		std::cout << "PRINTING ORIENTATION MATRIX BEFORE ROTATE:\n"
			<< orientationMatrix[0][0] << " " << orientationMatrix[1][0] << " " << orientationMatrix[2][0] << " " << orientationMatrix[3][0] << "\n"
			<< orientationMatrix[0][1] << " " << orientationMatrix[1][1] << " " << orientationMatrix[2][1] << " " << orientationMatrix[3][1] << "\n"
			<< orientationMatrix[0][2] << " " << orientationMatrix[1][2] << " " << orientationMatrix[2][2] << " " << orientationMatrix[3][2] << "\n"
			<< orientationMatrix[0][3] << " " << orientationMatrix[1][3] << " " << orientationMatrix[2][3] << " " << orientationMatrix[3][3] << "\n";

		orientationMatrix = glm::rotate(orientationMatrix, difference.x, glm::vec3(1.0f, 0.0f, 0.0f));
		orientationMatrix = glm::rotate(orientationMatrix, difference.y, glm::vec3(0.0f, 1.0f, 0.0f));
		orientationMatrix = glm::rotate(orientationMatrix, difference.z, glm::vec3(0.0f, 0.0f, 1.0f));

		std::cout << "PRINTING ORIENTATION MATRIX AFTER ROTATE:\n"
			<< orientationMatrix[0][0] << " " << orientationMatrix[1][0] << " " << orientationMatrix[2][0] << " " << orientationMatrix[3][0] << "\n"
			<< orientationMatrix[0][1] << " " << orientationMatrix[1][1] << " " << orientationMatrix[2][1] << " " << orientationMatrix[3][1] << "\n"
			<< orientationMatrix[0][2] << " " << orientationMatrix[1][2] << " " << orientationMatrix[2][2] << " " << orientationMatrix[3][2] << "\n"
			<< orientationMatrix[0][3] << " " << orientationMatrix[1][3] << " " << orientationMatrix[2][3] << " " << orientationMatrix[3][3] << "\n";

		std::cout << "ONLY ROTATION MATRIX:\n"
			<< getRotationMatrix()[0][0] << " " << getRotationMatrix()[1][0] << " " << getRotationMatrix()[2][0] << " " << getRotationMatrix()[3][0] << "\n"
			<< getRotationMatrix()[0][1] << " " << getRotationMatrix()[1][1] << " " << getRotationMatrix()[2][1] << " " << getRotationMatrix()[3][1] << "\n"
			<< getRotationMatrix()[0][2] << " " << getRotationMatrix()[1][2] << " " << getRotationMatrix()[2][2] << " " << getRotationMatrix()[3][2] << "\n"
			<< getRotationMatrix()[0][3] << " " << getRotationMatrix()[1][3] << " " << getRotationMatrix()[2][3] << " " << getRotationMatrix()[3][3] << "\n";
	}

	void BoxCollider::updateScale(glm::vec3 oldScale, glm::vec3 newScale)
	{
		glm::vec3 scalingFactor = newScale / oldScale;
		std::cout << "\nSCALE UPDATE:\nvec3(" << newScale.x << "," << newScale.y << "," << newScale.z
			<< ") / vec3(" << oldScale.x << "," << oldScale.y << "," << oldScale.z
			<< ") = vec3(" << scalingFactor.x << "," << scalingFactor.y << "," << scalingFactor.z << ")\n";
		orientationMatrix = glm::scale(orientationMatrix, scalingFactor);
		std::cout << "\nSCALING FACTOR: " << getScalingFactorFromMatrix().x << " " << getScalingFactorFromMatrix().y << " " << getScalingFactorFromMatrix().z << "\n";
		std::cout << "\nRADIUS UPDATE:\nold: " << radius.x << " " << radius.y << " " << radius.z;
		radius *= scalingFactor;
		std::cout << "\nnew: " << radius.x << " " << radius.y << " " << radius.z << "\n";
	}

	glm::vec3 BoxCollider::getRadius()
	{
		return radius;
	}

	glm::vec3 BoxCollider::getCenter()
	{
		return center;
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