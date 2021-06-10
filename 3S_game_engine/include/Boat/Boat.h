#pragma once
#ifndef BOAT_H
#define BOAT_H

#include "GameLogic/Components/Component.h"
#include "GameLogic/Proctor.h"

namespace GameLogic
{
	class Boat : public Component
	{
	public:
		Boat(GameLogic::ComponentType _type, GameLogic::Proctor* _proctor);

		void update();
		void attachPlayerOne(GameLogic::Proctor* player);
		void deatachPlayerOne(GameLogic::Proctor* player);
		void attachPlayerTwo(GameLogic::Proctor* player);
		void deatachPlayerTwo(GameLogic::Proctor* player);
		bool isPlayerOneInBoat();
		bool isPlayerTwoInBoat();
		void setIsPlayerOneInBoat(bool isFirstInBoat);
		void setIsPlayerTwoInBoat(bool isSecondInBoat);

	protected:

		InputSystem::KeyboardInput* keyboard;
	private:

		GameLogic::Proctor* parent = nullptr;
		GameLogic::Transform transform;

		bool isFirstInBoat = false;
		bool isSecondInBoat = false;

		float speed = 0;
		float startSpeed = 1;
		float turnSpeed = 0;
		float startTurningSpeed = 0.6;
		float time = 0;

		void useBoatInput();
		void changePosition();
	};
}

#endif // !BOAT_H