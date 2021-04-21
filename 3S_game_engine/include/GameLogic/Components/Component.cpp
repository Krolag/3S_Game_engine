#include "Component.h"
#include "Application/Randomizer.h"

namespace GameLogic
{
	Component::Component() { }

	Component::Component(ComponentType _type, Proctor* _proctor)
		: type(_type), proctor(_proctor)
	{
		uuid = uuid = Application::Randomizer().randomInt();
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
}