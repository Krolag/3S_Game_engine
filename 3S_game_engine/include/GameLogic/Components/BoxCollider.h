#pragma once
#ifndef BOXCOLLIDER_H
#define BOXCOLLIDER_H

#include "GameLogic/GameLogic.h"
#include "Component.h"
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

		void render();
	private:
		/* BoxCollider data */
		Loader::Model* model;
		glm::vec3 center;
		glm::vec3 radius;
		
		/* Render data */
		Shader* colliderShader;
		float colliderVertices[72];
		unsigned int colliderVAO, colliderVBO;
		
		/* Private methods */
		void initVerticesData();
		void extremePointsAlongDirection(glm::vec3 point, glm::vec3& min, glm::vec3& max);
		float lineOverlap(float minA, float maxA, float minB, float maxB);
	};
}

#endif // !BOXCOLLIDER_H