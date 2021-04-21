#pragma once
#ifndef HIERARCHY_H
#define HIERARCHY_H

#include "GameLogic/Proctor.h"
#include <vector>

namespace GameLogic
{
	class Hierarchy
	{
	public:
		/* Constructors */
		Hierarchy(bool _state = true);

		/* Objects method */
		void addObject(Proctor* _proctor);
		void removeObject(Proctor* _proctor);
		Proctor* getObject(std::string _name);
		Proctor* getObject(unsigned int _uuid);
		std::vector<Proctor*> getObjects();

		/* State methods */
		void setState(bool _state);
		bool getState();

		/* Render methods */
		void drawHierarchyWindow();

		void update();
		void cleanup();

	private:
		std::vector<Proctor*> objects;

		/* Debug window variables */
		bool active;
		std::string activeProctorName = "none";
	};
}

#endif // !HIERARCHY_H