#pragma once

#include "GameLogic/Proctor.h"
#include "GameLogic/Components/Component.h"
#include "InputSystem/KeyboardInput.h"

class Boat: GameLogic::Component
{
public:
	Boat(GameLogic::ComponentType _type, GameLogic::Proctor* _proctor);

	void update();

protected:

	InputSystem::KeyboardInput* keyboard;
private:

	GameLogic::Proctor* parent = nullptr;
	GameLogic::Transform transform;

	float speed = 0;
	float startSpeed = 5;
	float turnSpeed = 0;
	float startTurningSpeed = 0.5;

	void useBoatInput();

};

