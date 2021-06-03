#pragma once
#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include "Component.h"
#include "GameLogic/Proctor.h"
#include "InputSystem/KeyboardInput.h"
#include "Points/Points.h"
#include "../../IrrKlang/irrKlang.h"


namespace GameLogic
{	
	class Boat;

	class PlayerInput : public Component
	{
	public:
		float maxSpeed = 0.5f;
		float acceleration = 0.5f;
		float maxGravity = 20.0f;
		float gravityAcceleration = 0.0f; // 15.0f;
		float cGravity = 0.0f;
		float movementSpeed = 0;
		float maxInteractionDistance = 6.0f;
		float radius = maxInteractionDistance;
		int frameCounter = 0;
		Proctor* coinProctor;
		irrklang::ISoundEngine* playerSounds = irrklang::createIrrKlangDevice();

		/* Constructors */
		PlayerInput(ComponentType _type, Proctor* _proctor);
		PlayerInput(ComponentType _type, Proctor* _proctor, bool _isPlayerOne, Boat* _boat);

		/* Movement methods */
		void setScheme(bool _isPlayerOne);
		bool getScheme();

		void update();

	protected:
		InputSystem::KeyboardInput* keyboard;

	private:
		Proctor* parent = nullptr;
		Boat* boat;
		bool isPlayerOne;
		bool isPlayerOneInBoat = false;
		bool isPlayerTwoInBoat = false;

		void usePlayerOneInput();
		void usePlayerTwoInput();
	};
}
#endif // !PLAYERINPUT_H