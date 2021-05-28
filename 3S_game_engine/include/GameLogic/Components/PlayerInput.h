#pragma once
#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include "Component.h"
#include "GameLogic/Proctor.h"
#include "InputSystem/KeyboardInput.h"
#include "Points/Points.h"

namespace GameLogic
{
	class PlayerInput : public Component
	{
	public:
		float maxSpeed = 0.5f;
		float acceleration = 0.5f;
		float maxGravity = 20.0f;
		float gravityAcceleration = 0.0f; // 15.0f;
		float cGravity = 0.0f;
		float movementSpeed;
		float maxInteractionDistance = 3.0f;
		float radius = 3.0f;
		Proctor* coinProctor;

		/* Constructors */
		PlayerInput(ComponentType _type, Proctor* _proctor);
		PlayerInput(ComponentType _type, Proctor* _proctor, bool _isPlayerOne);

		/* Movement methods */
		void setScheme(bool _isPlayerOne);
		bool getScheme();

		void update();

	protected:
		InputSystem::KeyboardInput* keyboard;

	private:
		Proctor* parent = nullptr;
		bool isPlayerOne;

		void usePlayerOneInput();
		void usePlayerTwoInput();
	};
}
#endif // !PLAYERINPUT_H