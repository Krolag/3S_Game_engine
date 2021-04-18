#pragma once
#include "GameLogic/Proctor.h"
#include <vector>

class Hierarchy
{
public:
	/* Constructors */
	Hierarchy();

	/* Objects method */
	void addObject(Proctor* _proctor);
	void removeObject(Proctor* _proctor);
	Proctor* getObject(std::string _name);
	Proctor* getObject(unsigned int _uuid);
	std::vector<Proctor*> getObjects();
	
	void update();

private:
	std::vector<Proctor*> objects;
};