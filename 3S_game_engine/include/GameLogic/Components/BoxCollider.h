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
		Shader* colliderShader;

		/* Constructors */
		BoxCollider(ComponentType _type, Loader::Model* _model, Proctor* _proctor = NULL, Shader* _shader = NULL, bool _isStatic = true,
			glm::vec3 _position = glm::vec3(0.0f), glm::quat _rotation = glm::quat(1.0f, glm::vec3(0.0f)),
			glm::vec3 _scale = glm::vec3(1.0f));

		/* Set up methods */
		void init();
		void setModel(Loader::Model* _model);

		void render();
		void update();

		/* Variable methods */
		glm::vec3 getRadius();
		glm::mat4 getTranslateMatrix();
		glm::mat4 getScaleMatrix();

	protected:
		/* BoxCollider data */
		Loader::Model* model;
		glm::vec3 min;
		glm::vec3 max;
		glm::vec3 radius;

		/* Variables for drawing */
		float centerPoint[3];
		unsigned int colliderVBO, colliderVAO;

	private:
		void compareAndUpdateMin(glm::vec3 _value);
		void compareAndUpdateMax(glm::vec3 _value);
		void calcCenter();
		void calcRadius();
		std::vector<glm::vec3> getColliderBoxVertices();
		glm::mat4 getModelMatrix() const;
	};
}

#endif // !BOXCOLLIDER_H