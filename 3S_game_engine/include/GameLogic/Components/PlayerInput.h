#pragma once
#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include "Component.h"
#include "GameLogic/Proctor.h"
#include "InputSystem/KeyboardInput.h"
#include "Points/Points.h"
#include "UIRender/UIElement.h"
#include <string.h>
#include "../../IrrKlang/irrKlang.h"

//using namespace irrklang;

namespace GameLogic
{	
	class Boat;

	class PlayerInput : public Component
	{
	public:
		bool yUpDistance = false;
		bool yDownDistance = false;
		bool xRightDistance = false;
		bool xLeftDistance = false;
		bool isPlayerOneInBoat = false;
		bool isPlayerTwoInBoat = false;

		float maxSpeed = 0.5f;
		float acceleration = 0.5f;
		float maxGravity = 10.0f;
		float gravityAcceleration = 5.5f; // 15.0f;
		float cGravity = 0.0f;
		float movementSpeed = 0;
		float maxInteractionDistance = 6.0f;
		float maxBoatInteractionDistance = 15.0f;
		float radius = maxInteractionDistance;
		int frameCounter = 0;
		
		UIRender::UIElement* controls[6];

		//colect chest TEST
		float timepassed = 0;
		int buttonToPress;
		bool isPlayerOneUsingChest = false;

		int clue;
		bool isFinalChestOpen = false;

		Proctor* coinProctor;
		irrklang::ISoundEngine* playerSounds = irrklang::createIrrKlangDevice();
		std::vector<irrklang::ISoundSource*> footsteps = { 
			playerSounds->addSoundSourceFromFile("assets/audio/sounds/footstep_00.ogg"),
			playerSounds->addSoundSourceFromFile("assets/audio/sounds/footstep_01.ogg"),
			playerSounds->addSoundSourceFromFile("assets/audio/sounds/footstep_02.ogg"),
			playerSounds->addSoundSourceFromFile("assets/audio/sounds/footstep_03.ogg")
		};
		std::string clueText;
		bool isCluePickedUp = false;
		bool isFirstClicked = false;
		bool stayCloseToInteractable = false;
		//int clues[5]{ 0, 1, 2, 3, 4 };

		/* Constructors */
		PlayerInput(ComponentType _type, Proctor* _proctor);
		PlayerInput(ComponentType _type, Proctor* _proctor, bool _isPlayerOne, Boat* _boat);

		std::vector<irrklang::ISoundSource*> getPlayerSounds();

		/* Movement methods */
		void setScheme(bool _isPlayerOne);
		bool getScheme();

		void update();

		void checkForDistance();
		float closestDistance;
		float closestIndex;

	protected:
		InputSystem::KeyboardInput* keyboard;

	private:
		Proctor* parent = nullptr;
		Boat* boat;
		bool isPlayerOne;


		void usePlayerOneInput();
		void usePlayerTwoInput();
		void primaryButtonInUse();

		//Chest testing
		void openChest();
		bool isChestOpen = false;
		int randomNumber = 0;
		std::string input;
	};
}
#endif // !PLAYERINPUT_H