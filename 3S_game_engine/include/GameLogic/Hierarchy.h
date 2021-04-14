#pragma once
#include "GameLogic/Proctor.h"
#include <vector>

class Hierarchy
{
public:
	Hierarchy();

	void addObject(Proctor _proctor);
	void removeObject(Proctor _proctor);
	Proctor getObject(std::string _name);
	std::vector<Proctor> getObjects();

private:
	std::vector<Proctor> objects;
};