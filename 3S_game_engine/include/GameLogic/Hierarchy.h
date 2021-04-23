#pragma once
#ifndef HIERARCHY_H
#define HIERARCHY_H

#include "Application/Application.h"
#include "GameLogic/Proctor.h"
#include <vector>

namespace GameLogic
{
	class Hierarchy
	{
	public:
		/* Constructors */
		Hierarchy(bool _state = true);
		Hierarchy(Application::Scene* _scene, bool _state = true);

		/* Objects method */
		void addObject(Proctor* _proctor);
		void removeObject(Proctor* _proctor);
		Proctor* getObject(std::string _name);
		Proctor* getObject(unsigned int _uuid);
		std::vector<Proctor*> getObjects();

		/* Scene method */
		float getDeltaTime();

		/* State methods */
		void setState(bool _state);
		bool getState();

		/* Render methods */
		void drawHierarchyWindow();

		void update();
		void cleanup();

	private:
		std::vector<Proctor*> objects;
		Application::Scene* scene;

		/* Debug window variables */
		bool active;
		std::string activeProctorName = "none";
	};
}

#endif // !HIERARCHY_H