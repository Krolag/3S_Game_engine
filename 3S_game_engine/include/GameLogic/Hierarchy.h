#pragma once
#include "GameLogic/Proctor.h"
#include <vector>

class Hierarchy
{
public:
	Hierarchy();

	void addObject(Proctor* _proctor);
	void removeObject(Proctor* _proctor);
	Proctor* getObject(std::string _name);
	Proctor* getObject(unsigned int uuid);
	std::vector<Proctor*> getObjects();
	void update();

private:
	std::vector<Proctor*> objects;
};