#include "Hierarchy.h"

Hierarchy::Hierarchy() { }

void Hierarchy::addObject(Proctor* _proctor)
{
	/* Check if given ohject is already assigned to hierarchy */
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == _proctor)
		{
			return;
		}
	}

	/* If none found, assing it to objects vector */
	objects.push_back(_proctor);
}

void Hierarchy::removeObject(Proctor* _proctor)
{
	/* Check if any given proctor is in use */
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		/* If given proctor is in use, remove it */
		if (objects[i] == _proctor)
		{
			objects.erase(objects.begin() + i);
		}
	}
}

Proctor* Hierarchy::getObject(std::string _name)
{
	/* Check if proctor with given name is in use */
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		/* If proctor with given name is in use, return it */
		if (objects[i]->name == _name)
		{
			return objects[i];
		}
	}
}

Proctor* Hierarchy::getObject(unsigned int _uuid)
{
	/* Check if proctor with given uuid is in use */
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		/* If proctor with given uuid is in use, return it */
		if (objects[i]->uuid == _uuid)
		{
			return objects[i];
		}
	}
}

std::vector<Proctor*> Hierarchy::getObjects()
{
	return objects;
}

void Hierarchy::update()
{
	/* Update all proctors in objects vector */
	for (auto& a : objects)
	{
		a->update();
	}
}
