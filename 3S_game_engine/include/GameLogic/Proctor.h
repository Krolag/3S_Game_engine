#pragma once
#ifndef PROCTOR_H
#define PROCTOR_H

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>
#include <vector>
#include "Loader/Loader.h"
#include "Shader/Shader.h"
#include "Components/MeshRenderer.h"
#include "Components/PlayerInput.h"
#include "Components/BoxCollider.h"
#include "Components/Interactable.h"

namespace GameLogic
{
	class Hierarchy;

	struct Transform
	{
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};

	class Proctor
	{
	public:
		std::string name = "none";
		unsigned int uuid = 0;
		Transform transform;

		/* Constructors / Destructors */
		Proctor();
		Proctor(const char* _name, glm::vec3 _position = glm::vec3(0.0f), glm::quat _rotation = glm::quat(1.0f, glm::vec3(0.0f)), 
			glm::vec3 _scale = glm::vec3(1.0f), Proctor* _parent = NULL, unsigned int _uuid = 0,
			bool _active = true, bool _isStatic = false);
		~Proctor();

		/* Children methods */
		void addChild(Proctor* _proctor);
		void removeChild(Proctor* _proctor);
		void removeChildren();
		Proctor* getParentProctor() const;
		Hierarchy* getParentHierarchy() const;
		void setParent(Proctor* _proctor);
		void setParent(Hierarchy* _hierarchy);
		unsigned int childCount();

		/* Transform methods */
		void setTransform(glm::vec3 _position = glm::vec3(1.0f), glm::quat _rotation = glm::quat(glm::vec3(1.0f)), glm::vec3 _scale = glm::vec3(1.0f));
		void setTransform(Transform _transform);
		void setPosition(glm::vec3 _position);
		void setRotation(glm::quat _rotation);
		void setScale(glm::vec3 _scale);
		Transform getTransform();
		glm::vec3 getPosition();
		glm::quat getRotation();
		glm::vec3 getScale();

		/* Components methods */
		void addComponent(Component* _component);
		Component* getComponentOfType(ComponentType _type);
		void setDeltaTime(float _value);
		float getDeltaTime();

		/* Debug window methods */
		void drawDebugWindow();

		void update();
		void cleanup();

	private:
		/* Hierarchy data */
		Proctor* parentProctor = NULL;
		Hierarchy* parentHierarchy = NULL;
		std::vector<Proctor*> children;

		/* Proctor data */
		bool active = true;
		bool isStatic = false;
		float deltaTime;
		std::vector<Component*> components;
	};
}

#endif // !PROCTOR_H