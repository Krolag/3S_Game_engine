#include "Component.h"
#include "GameLogic/Randomizer.h"

Component::Component() { }

Component::Component(ComponentType _type, Proctor* _proctor)
	: type(_type), proctor(_proctor)
{
	uuid = uuid = Randomizer().randomInt();
}

Component::~Component() { }

bool Component::isActive()
{
	return active;
}

void Component::setActive(bool _value)
{
	active = _value;
}
