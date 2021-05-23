#pragma once
#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "Component.h"
#include "GameLogic/Proctor.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GameLogic
{
	class BoxCollider : public Component
	{
	public:
		bool isStatic;
		bool isUpdated;
		bool isColliding;
		
		/* Constructors */
		BoxCollider(ComponentType _type, Loader::Model* _model, Proctor* _proctor = NULL, Shader* _shader = NULL, bool _isStatic = true,
			glm::vec3 _position = glm::vec3(0.0f), glm::quat _rotation = glm::quat(1.0f, glm::vec3(0.0f)),
			glm::vec3 _scale = glm::vec3(1.0f));

		/* Set up methods */
		void init();
		void setModel(Loader::Model* _model);
		bool checkCollisionOBB(BoxCollider* otherCollider);

		/* Update methods */
		void update();

		/* Getters */
		glm::vec3 getRadius();
		glm::vec3 getCenter();
		std::vector<glm::vec3> getColliderVertices();
		
		glm::mat4 getTranslationMatrix();
		glm::mat4 getRotationMatrix();
		glm::vec3 getScalingFactorFromMatrix();

		void render();
	private:
		/* BoxCollider data */
		Loader::Model* model;
		glm::vec3 center;
		glm::vec3 radius;
		glm::mat4 orientationMatrix;
		
		/* Render data */
		Shader* colliderShader;
		float colliderVertices[72];
		unsigned int colliderVAO, colliderVBO;
		
		/* Private methods */
		void initVerticesData();
		void extremePointsAlongDirection(glm::vec3 point, glm::vec3& min, glm::vec3& max);
		bool isBetweenOrdered(float val, float lowerBound, float upperBound);
		std::vector<glm::vec3> calcVertices();
		glm::mat4 getModelMatrix() const;
	};
}

#endif // !BOXCOLLIDER_H