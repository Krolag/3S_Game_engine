#pragma once
#ifndef PROCTOR_H
#define PROCTOR_H
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/matrix.hpp>
#include "Components/Component.h"
#include <vector>
#include "Loader/Loader.h"
#include "Shader/Shader.h"

struct Transform
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
};

class Proctor
{
public:
	Transform transform;
	std::string name;

	Proctor(
		std::string _name,
		const glm::vec3& _position = glm::vec3(0.0f),
		const glm::vec3& _rotation = glm::vec3(0.0f),
		const glm::vec3& _scale = glm::vec3(1.0f)
		);
	~Proctor();

	void addChild(Proctor _child);
	Proctor getChild(Proctor _child);
	std::vector<Proctor> getChildren();
	void addComponent(Component _component);
	void input();
	void update();
	void render();

	Transform getTransform() { return transform; }
	void setScale(glm::vec3 _scale);

private:
	std::vector<Proctor> children;
	std::vector<Component> components;
};

#endif // !PROCTOR_H