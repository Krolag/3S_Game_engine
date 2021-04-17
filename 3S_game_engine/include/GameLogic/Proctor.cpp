#include "Proctor.h"
#include <iostream>
#include "GameLogic/Randomizer.h"

Proctor::Proctor(const char* _name, unsigned int _uuid, Proctor* _parent, bool _active, bool _isStatic)
	: name(_name), uuid(_uuid), parent(_parent), active(_active), isStatic(_isStatic)
{
	if (uuid == 0)
	{
		uuid = uuid = Randomizer().randomInt();
	}
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

void Proctor::addComponent(Component* _component)
{
	components.push_back(_component);
}
