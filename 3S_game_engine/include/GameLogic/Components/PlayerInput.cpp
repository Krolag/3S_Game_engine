#include "PlayerInput.h"
#include "GameLogic/Hierarchy.h"
#include "GameLogic/Components/Cash.h"
#include "Boat/Boat.h"
#include "Anima.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

//using namespace irrklang;

namespace GameLogic
{
	PlayerInput::PlayerInput(ComponentType _type, Proctor* _proctor)
		: Component(_type, _proctor), parent(_proctor)
	{ 
		if (_proctor != NULL)
		{
			srand(time(NULL));
			proctor->addComponent(this);
		}
	}

	PlayerInput::PlayerInput(ComponentType _type, Proctor* _proctor, bool _isPlayerOne, Boat* _boat)
		: Component(_type, _proctor), parent(_proctor), isPlayerOne(_isPlayerOne), boat(_boat)
	{ 
		if (_proctor != NULL)
		{
			srand(time(NULL));
			proctor->addComponent(this);
		}
	}

	std::vector<irrklang::ISoundSource*> PlayerInput::getPlayerSounds()
	{
		return footsteps;
	}

	/*void PlayerInput::setFootstepSound(std::vector<ISoundSource*> _footsteps)
	{
		for (int i = 0; i < 4; i++)
		{
			footsteps.at(i) = _footsteps.at(i);
		}
	}*/

	/*ISoundSource** PlayerInput::getFootstepSound()
	{
		return footsteps;
	}*/

	void PlayerInput::setScheme(bool _isPlayerOne)
	{
		isPlayerOne = _isPlayerOne;
	}

	bool PlayerInput::getScheme()
	{
		return isPlayerOne;
	}

	void PlayerInput::update()
	{
		frameCounter++;
		isPlayerOneInBoat = boat->isPlayerOneInBoat();
		isPlayerTwoInBoat = boat->isPlayerTwoInBoat();

		checkForDistance();

		/* Apply gravity */
		cGravity += gravityAcceleration * proctor->getDeltaTime();
		if (cGravity >= maxGravity)
			cGravity = maxGravity;

		randomNumber = rand() % 4;

		/* Check if player should take damage from drowning */
		if (proctor->transform.position.y < 0.0f)
		{
			if (isPlayerOne)
				playerSounds->play2D(drown_D, false);
			else
				playerSounds->play2D(drown_K, false);
			proctor->getParentHierarchy()->takeDamage();
			proctor->transform.position.y = proctor->getParentHierarchy()->getCamera()->Position.y;
		}

		/* Check which player should get update */
		if (isPlayerOne) 
			usePlayerOneInput();
		else
			usePlayerTwoInput();
	}

	void PlayerInput::checkForDistance()
	{
		std::vector<Proctor*> tmp = proctor->getParentHierarchy()->getInteractable();

		/* Initialize first tmp values as start distance and index */
		float xDistance = tmp.at(0)->getPosition()[0] - proctor->getPosition()[0];
		float zDistance = tmp.at(0)->getPosition()[2] - proctor->getPosition()[2];
		closestDistance = sqrt(xDistance * xDistance + zDistance * zDistance);
		closestIndex = 0;
		unsigned int size = tmp.size();

		/* Calculate distance for each interactable proctor */
		for (unsigned int i = 1; i < size; i++)
		{
			float xDistance = tmp.at(i)->getPosition()[0] - proctor->getPosition()[0];
			float zDistance = tmp.at(i)->getPosition()[2] - proctor->getPosition()[2];
			float distance = sqrt(xDistance * xDistance + zDistance * zDistance);

			if (distance < closestDistance)
			{
				closestDistance = distance;
				closestIndex = i;
			}
		}

		stayCloseToInteractable = false;
		if (closestDistance < maxBoatInteractionDistance && tmp.at(closestIndex)->name == "boat")
			stayCloseToInteractable = true;
		if (closestDistance < maxInteractionDistance)
			stayCloseToInteractable = true;
	}

	void PlayerInput::usePlayerOneInput()
	{
		if (isActive())
		{
			/* Get deltaTime from proctor */
			float deltaTime = proctor->getDeltaTime();
			/* Get proctors parent transform */
			Transform transform = proctor->getTransform();

			if (!isPlayerOneInBoat)
			{
				/* Calculate speed */
				movementSpeed += acceleration * deltaTime;
				if (movementSpeed >= maxSpeed)
				{
					movementSpeed = maxSpeed;
				}

				/* If any movement key is used, play sound and animation */
				if ((keyboard->isKeyDown(GLFW_KEY_W) || 
					keyboard->isKeyDown(GLFW_KEY_S) || 
					keyboard->isKeyDown(GLFW_KEY_A) || 
					keyboard->isKeyDown(GLFW_KEY_D)))
				{
					/* Check if player clicked any button */
					if (!isFirstClicked)
						isFirstClicked = true;

					currentTime += deltaTime;
					if (currentTime > 0.4f)
					{
						currentTime = 0.0f;
						playerSounds->play2D(footsteps.at(randomNumber));
					}

					//if (proctor->getComponentOfType(C_ANIMA) != NULL)
					//	((Anima*)proctor->getComponentOfType(C_ANIMA))->playAnimation(-1);
				}

				/* Horizontal movement */
				if (keyboard->isKeyDown(GLFW_KEY_A) && xLeftDistance)
				{
					transform.position += glm::vec3(-movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(89.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(89.5));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_D) && xRightDistance)
				{
					transform.position += glm::vec3(movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(269.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-89.5f));
				}

				/* Vertical movement */
				if (keyboard->isKeyDown(GLFW_KEY_W) && yUpDistance)
				{
					transform.position += glm::vec3(0.0f, 0.0f, -movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_A) && xLeftDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(134.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_D) && xRightDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(224.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(0.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(179.5f));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_S) && yDownDistance)
				{
					transform.position += glm::vec3(0.0f, 0.0f, movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_A) && xLeftDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(134.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(44.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_D) && xRightDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-134.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-44.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(179.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, 0.0f);
				}
			}

			/* Collect players primary and secondary button info */
			if (keyboard->isKeyReleased(GLFW_KEY_V))
			{
				whichPlayerInteracts = 1;
				primaryButtonInUse();
			}
			if (keyboard->isKeyPressed(GLFW_KEY_B) && isPlayerOneInBoat)
			{
				boat->detachPlayerOne(proctor);
				transform.position = proctor->transform.position;
				transform.rotation = proctor->transform.rotation;
			}

			/* Check if player is holding any movement button */
			if (!keyboard->isKeyDown(GLFW_KEY_W) && !keyboard->isKeyDown(GLFW_KEY_S)
				&& !keyboard->isKeyDown(GLFW_KEY_A) && !keyboard->isKeyDown(GLFW_KEY_D))
			{
				movementSpeed = 0.0f;
			}

			/* Update gravity */
			transform.position += glm::vec3(0.0f, -cGravity, 0.0f) * proctor->getDeltaTime();

			/* Update players transform */
			proctor->setTransform(transform);
			((MeshRenderer*)proctor->getComponentOfType(C_MESH))->model->position = transform.position;
			((MeshRenderer*)proctor->getComponentOfType(C_MESH))->model->rotation = transform.rotation;
			((MeshRenderer*)proctor->getComponentOfType(C_MESH))->model->scale = transform.scale;
		}
	}

	void PlayerInput::usePlayerTwoInput()
	{
		if (isActive())
		{
			/* Get deltaTime from proctor */
			float deltaTime = proctor->getDeltaTime();

			/* Get proctors parent transform */
			Transform transform = proctor->getTransform();

			//randomNumber = ran

			if (!isPlayerTwoInBoat)
			{
				/* Calculate speed */
				movementSpeed += acceleration * deltaTime;
				if (movementSpeed >= maxSpeed)
				{
					movementSpeed = maxSpeed;
				}

				if ((keyboard->isKeyDown(GLFW_KEY_I) ||
					keyboard->isKeyDown(GLFW_KEY_K) ||
					keyboard->isKeyDown(GLFW_KEY_J) ||
					keyboard->isKeyDown(GLFW_KEY_L)))
				{
					if (!isFirstClicked)
						isFirstClicked = true;
					
					currentTime += deltaTime;
					if (currentTime > 0.4f)
					{
						currentTime = 0.0f;
						playerSounds->play2D(footsteps.at(randomNumber));
					}

					//((Anima*)proctor->getComponentOfType(C_ANIMA))->playAnimation(1);
				}

				/* Horizontal movement */
				if (keyboard->isKeyDown(GLFW_KEY_J) && xRightDistance)
				{
					transform.position += glm::vec3(-movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(89.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(89.5));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_L) && xLeftDistance)
				{
					transform.position += glm::vec3(movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(269.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-89.5f));
				}

				/* Vertical movement */
				if (keyboard->isKeyDown(GLFW_KEY_I) && yDownDistance)
				{
					transform.position += glm::vec3(0.0f, 0.0f, -movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_J) && xRightDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(134.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_L) && xLeftDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(224.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(0.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(179.5f));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_K) && yUpDistance)
				{
					transform.position += glm::vec3(0.0f, 0.0f, movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_J) && xRightDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(134.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(44.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_L) && xLeftDistance)
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-134.5f), 0.0f);

						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-44.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(179.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, 0.0f);
				}
			}

			/* Collect players update */
			if (keyboard->isKeyReleased(GLFW_KEY_PERIOD))
			{
				whichPlayerInteracts = 2;
				primaryButtonInUse();
			}

			if (keyboard->isKeyPressed(GLFW_KEY_SLASH) && isPlayerTwoInBoat)
			{
				boat->detachPlayerTwo(proctor);
				transform.position = proctor->transform.position;
				transform.rotation = proctor->transform.rotation;
			}

			/* Check if player is holding any movement button */
			if (!keyboard->isKeyDown(GLFW_KEY_I) && !keyboard->isKeyDown(GLFW_KEY_K)
				&& !keyboard->isKeyDown(GLFW_KEY_J) && !keyboard->isKeyDown(GLFW_KEY_L))
			{
				movementSpeed = 0.0f;
			}

			/* Update gravity */
			transform.position += glm::vec3(0.0f, -cGravity, 0.0f) * proctor->getDeltaTime();

			/* Update players transform */
			proctor->setTransform(transform);
			((MeshRenderer*)proctor->getComponentOfType(C_MESH))->model->position = transform.position;
			((MeshRenderer*)proctor->getComponentOfType(C_MESH))->model->rotation = transform.rotation;
			((MeshRenderer*)proctor->getComponentOfType(C_MESH))->model->scale = transform.scale;
		}
	}

	void PlayerInput::primaryButtonInUse()
	{
		/* Create tmp for easy acces */
		std::vector<Proctor*> tmp = proctor->getParentHierarchy()->getInteractable();
		isCluePickedUp = false;

		/* Check if closest thing is boat */
		if (tmp.at(closestIndex)->name == "boat" && closestDistance <= maxBoatInteractionDistance)
		{
			// todo: @Dawid fix for second player
			if (!isPlayerOneInBoat && isPlayerOne)
			{
				boat->attachPlayerOne(proctor);
			}
			if (!isPlayerTwoInBoat && !isPlayerOne)
			{
				boat->attachPlayerTwo(proctor);
			}
		}
		/* Else check which type of object is the closest */
		else if (closestDistance <= maxInteractionDistance)
		{
			if (whichPlayerInteracts == 1)
				playerSounds->play2D(chest_D, false);
			else if (whichPlayerInteracts == 2)
				playerSounds->play2D(chest_K, false);

			if (tmp.at(closestIndex)->getComponentOfType(C_TREASURE) != NULL) //&& isChestOpen
			{
				tmp.at(closestIndex)->deactivate();
				((Cash*)this->proctor->getParentHierarchy()->getObjectsInRadiusOf(this->proctor, radius)->getComponentOfType(GameLogic::C_CASH))->setFollow(true);
			}

			// finalChest
			if (tmp.at(closestIndex)->getComponentOfType(C_INTERACTABLE) != NULL && tmp.at(closestIndex)->name == "finalChest-1")
			{
				tmp.at(closestIndex)->deactivate();
				isFinalChestOpen = true;
			}

			if (tmp.at(closestIndex)->getComponentOfType(C_INTERACTABLE) != NULL && tmp.at(closestIndex)->getComponentOfType(C_INTERACTABLE)->isActive() == true)
			{
				if (tmp.at(closestIndex)->name == "x-1" || tmp.at(closestIndex)->name == "x-2" || tmp.at(closestIndex)->name == "x-3" || tmp.at(closestIndex)->name == "x-4" || tmp.at(closestIndex)->name == "x-5")
				{
					tmp.at(closestIndex)->deactivate();
					/*tmp.at(i)->getComponentOfType(C_INTERACTABLE)->setActive(false);
					tmp.at(i)->getParentHierarchy()->removeObject(tmp.at(i));*/
					if (tmp.at(closestIndex)->name == "x-1")
						//clueText = "The winds blow NORTH";
						clue = 0;
					if (tmp.at(closestIndex)->name == "x-2")
						//clueText = "The winds blow NORTH-WEST";
						clue = 1;
					if (tmp.at(closestIndex)->name == "x-3")
						//clueText = "The winds blow SOUTH-WEST";
						clue = 2;
					if (tmp.at(closestIndex)->name == "x-4")
						//clueText = "The winds blow SOUTH";
						clue = 3;
					if (tmp.at(closestIndex)->name == "x-5")
						clueText = "Familiar odor...?";

					isCluePickedUp = true;
				}
			}
		}
	}

	void PlayerInput::openChest()
	{
		timepassed += proctor->getParentHierarchy()->getDeltaTime();
		if (timepassed > 0.8) {
			timepassed = 0;
		}
		if (keyboard->isKeyPressed(GLFW_KEY_V) && buttonToPress==0) {
			input.append(std::to_string(0));
			timepassed = 0;
		}
		else if(keyboard->isKeyPressed(GLFW_KEY_V) && !buttonToPress == 0)
		{
			input = "";
		}
		std::cout << input << "\n";
		if (input.compare("000") == 0) { 
			isChestOpen = true;
			isPlayerOneUsingChest = false;
		}
	}
}