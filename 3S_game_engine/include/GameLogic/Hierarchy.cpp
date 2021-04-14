#include "Hierarchy.h"

Hierarchy::Hierarchy()
{
}

void Hierarchy::addObject(Proctor _proctor)
{
	objects.push_back(_proctor);
}

void Hierarchy::removeObject(Proctor _proctor)
{
	// TODO : Remove objects
}

Proctor Hierarchy::getObject(std::string _name)
{
	for (auto obj : objects)
	{
		if (obj.name == _name)
			return obj;
	}

	return Proctor("");
}

std::vector<Proctor> Hierarchy::getObjects()
{
	return objects;
}
