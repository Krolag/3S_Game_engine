#pragma once
#ifndef COMPONENT_H

enum ComponentType
{
	C_MESH = 1,
	C_MATERIAL = 2
};

class Proctor;

class Component
{
public:
	Proctor* proctor;

	/* Constructors / Destructors */
	Component();
	Component(ComponentType _type, Proctor* _proctor);
	virtual ~Component();

	virtual void update() {}

	bool isActive();
	void setActive(bool _value);

protected:
	bool active = true;
	ComponentType type;
	unsigned int uuid = 0;
};

#endif // !COMPONENT_H
