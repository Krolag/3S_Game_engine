#include "Hierarchy.h"

Hierarchy::Hierarchy()
{
}

void Hierarchy::addObject(Proctor* _proctor)
{
	objects.push_back(_proctor);
}

void Hierarchy::update()
{
	for (auto& a : objects)
	{
		a->update();
	}
}
