#pragma once
#ifndef HIERARCHY_H
#define HIERARCHY_H

#include "Application/Application.h"
#include "GameLogic/Proctor.h"
#include "Camera/Camera.h"
#include <vector>

namespace GameLogic
{
	class Hierarchy
	{
	public:
		/* Constructors */
		Hierarchy(bool _state = true);
		Hierarchy(Application::Scene* _scene, Camera* _camera = NULL, bool _state = true);

		/* Objects method */
		void addObject(Proctor* _proctor);
		void removeObject(Proctor* _proctor);
		Proctor* getObject(std::string _name);
		Proctor* getObject(unsigned int _uuid);
		std::vector<Proctor*> getProctors();
		std::vector<Proctor*> getInteractable();
		std::vector<Proctor*> getTreasure();
		std::vector<Proctor*> getCash();

		/* Collider methods */
		bool checkAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b);
		void separateAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b);

		/* Scene method */
		float getDeltaTime();

		/* Camera method */
		void setCamera(Camera* _camera);
		Camera* getCamera();

		/* State methods */
		void setState(bool _state);
		bool getState();

		/* Render methods */
		void drawHierarchyWindow();

		void update(bool _onlyRender = false, bool _drawDebug = false);
		void cleanup();

	private:
		bool collidersLoaded;
		/* TODO: @Ignacy - znajdz lepszy sposob na dostep do danych, bez tworzenia kolejnych vectorow */
		// Szanujmy pamiec
		std::vector<Proctor*> proctors;
		std::vector<Proctor*> interactable;
		std::vector<Proctor*> treasure;
		std::vector<Proctor*> cash;
		Application::Scene* scene;
		Camera* camera;

		/* Debug window variables */
		bool active;
		std::string activeProctorName = "none";
	};
}

#endif // !HIERARCHY_H