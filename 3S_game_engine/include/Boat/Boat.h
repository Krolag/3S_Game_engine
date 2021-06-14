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
		void detachPlayerOne(GameLogic::Proctor* player);
		void attachPlayerTwo(GameLogic::Proctor* player);
		void detachPlayerTwo(GameLogic::Proctor* player);
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

		float waveRotation = 0.0012;
		float speed = 0;
		float turnSpeed = 0;
		float startSpeed = 2;
		float maxTurningSpeed = 0.4;
		float time = 0;
		float deltaTime;

		bool rightTurn = false;
		bool leftTurn = false;

		void useBoatInput();
		void changePosition();
	};
}

#endif // !BOAT_H