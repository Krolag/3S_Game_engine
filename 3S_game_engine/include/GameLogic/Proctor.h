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
	std::string name;
	Transform transform;

	Proctor();
	Proctor(Proctor* _parent);
	Proctor(const char* _name, unsigned int _uuid, Proctor* _parent, bool _active = true, bool _isStatic = false);
	~Proctor();

	void update();

	/* Children methods */
	void addChild(Proctor* _proctor);
	void removeChild(Proctor* _proctor);
	void removeChildren();
	Proctor* getParent() const;
	void setParent(Proctor* _proctor);
	unsigned int childCount();

	/* Components methods */
	void addComponent(Component* _component);

private:
	/* Hierarchy data */
	Proctor* parent = NULL;
	std::vector<Proctor*> children;

	/* Proctor data */
	unsigned int uuid = 0;
	bool active = true;
	bool isStatic = false;
	std::vector<Component*> components;
};

#endif // !PROCTOR_H