#include "Proctor.h"
#include <iostream>
#include "GameLogic/Randomizer.h"

Proctor::Proctor() 
{ 
	/* Assign values to transform */
	transform.position = glm::vec3(0.0f);
	//transform.rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	transform.scale = glm::vec3(1.0f);
}

Proctor::Proctor(const char* _name, unsigned int _uuid, Proctor* _parent, bool _active, bool _isStatic)
	: name(_name), uuid(_uuid), parent(_parent), active(_active), isStatic(_isStatic)
{
	/* Assign uuid */
	if (uuid == 0)
	{
		uuid = uuid = Randomizer().randomInt();
	}
	
	/* Assign values to transform */
	transform.position = glm::vec3(0.0f);
	//transform.rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	transform.scale = glm::vec3(1.0f);
}

Proctor::~Proctor() { }

void Proctor::update()
{
	/* First update components for current hierarchy level */
	for (auto& a : components)
	{
		a->update();
	}

	/* Next update children */
	for (auto &a : children)
	{
		a->update();
	}
}

void Proctor::addChild(Proctor* _proctor)
{
	/* Check if given proctor is already in use */
	for (unsigned int i = 0; i < children.size(); i++)
	{
		if (children[i] == _proctor)
		{
			return;
		}
	}

	/* If proctor cannot be found in children vector, add it */
	children.push_back(_proctor);
}

void Proctor::removeChild(Proctor* _proctor)
{
	/* Find correct proctor and delete it */
	for (unsigned int i = 0; i < children.size(); i++)
	{
		if (children[i] == _proctor)
		{
			children.erase(children.begin() + i);
			return;
		}
	}
}

void Proctor::removeChildren()
{
	/* Drop all children */
	children.clear();
}

Proctor* Proctor::getParent() const
{
	return parent;
}

void Proctor::setParent(Proctor* _proctor)
{
	parent = _proctor;
}

unsigned int Proctor::childCount()
{
	return children.size();
}

void Proctor::addComponent(Component* _component)
{
	/* Check if given component is already in use */
	for (unsigned int i = 0; i < components.size(); i++)
	{
		if (components[i] == _component)
		{
			return;
		}
	}

	/* If component cannot be found in components vector, add it */
	components.push_back(_component);
}

void Proctor::setTransform(glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
{
	transform.position = _position;
	transform.rotation = _rotation;
	transform.scale = _scale;
}

void Proctor::setPosition(glm::vec3 _position)
{
	transform.position = _position;
}

void Proctor::setRotation(glm::quat _rotation)
{
	transform.rotation = _rotation;
}

void Proctor::setScale(glm::vec3 _scale)
{
	transform.scale = _scale;
}

Transform Proctor::getTransform()
{
	return transform;
}

glm::vec3 Proctor::getPosition()
{
	return transform.position;
}

glm::quat Proctor::getRotation()
{
	return transform.rotation;
}

glm::vec3 Proctor::getScale()
{
	return transform.scale;
}
