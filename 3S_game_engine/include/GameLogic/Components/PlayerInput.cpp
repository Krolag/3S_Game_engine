#include "PlayerInput.h"
#include "GameLogic/Hierarchy.h"
#include "GameLogic/Components/Cash.h"
#include "Boat/Boat.h"
#include "Anima.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

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

		/* Apply gravity */
		cGravity += gravityAcceleration * proctor->getDeltaTime();
		if (cGravity >= maxGravity)
			cGravity = maxGravity;

		randomNumber = rand() % 4;

		/* Check which player should get update */
		if (isPlayerOne) 
			usePlayerOneInput();
		else
			usePlayerTwoInput();
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
					keyboard->isKeyDown(GLFW_KEY_D)) && 
					frameCounter % 10 == 0)
				{
					if (randomNumber == 0)
						playerSounds->play2D("assets/audio/sounds/footstep_00.ogg", false);
					else if (randomNumber == 1)
						playerSounds->play2D("assets/audio/sounds/footstep_01.ogg", false);
					else if (randomNumber == 2)
						playerSounds->play2D("assets/audio/sounds/footstep_02.ogg", false);
					else if (randomNumber == 3)
						playerSounds->play2D("assets/audio/sounds/footstep_03.ogg", false);

					//if (proctor->getComponentOfType(C_ANIMA) != NULL)
					//	((Anima*)proctor->getComponentOfType(C_ANIMA))->playAnimation(-1);
				}

				/* Horizontal movement */
				if (keyboard->isKeyDown(GLFW_KEY_A))
				{
					transform.position += glm::vec3(-movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(269.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(89.5));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_D))
				{
					transform.position += glm::vec3(movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(89.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-89.5f));
				}

				/* Vertical movement */
				if (keyboard->isKeyDown(GLFW_KEY_W))
				{
					transform.position += glm::vec3(0.0f, 0.0f, -movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_A))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-134.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(134.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_D))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(134.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(224.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(179.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(179.5f));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_S))
				{
					transform.position += glm::vec3(0.0f, 0.0f, movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_A))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(44.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_D))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-44.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(0.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, 0.0f);
				}
			}

			/* Collect players primary and secondary button info */
			if (keyboard->isKeyReleased(GLFW_KEY_V))
			{
				/* Create tmp for easy acces */
				std::vector<Proctor*> tmp = proctor->getParentHierarchy()->getInteractable();
				isCluePickedUp = false;

				/* Check if player is near the interactable proctor */
				for (unsigned int i = 0; i < tmp.size(); i++)
				{
					float xDistance = tmp.at(i)->getPosition()[0] - proctor->getPosition()[0];
					float zDistance = tmp.at(i)->getPosition()[2] - proctor->getPosition()[2];
					float distance = sqrt(xDistance * xDistance + zDistance * zDistance);
					//std::cout << tmp.at(i)->name << ": " << distance << std::endl;

					if (distance <= maxInteractionDistance)
					{
						if (tmp.at(i)->name == "boat")
						{
							if (!isPlayerOneInBoat)
							{
								boat->attachPlayerOne(proctor);
								boat->setIsPlayerOneInBoat(true);
							}
						}

						if (tmp.at(i)->getComponentOfType(C_TREASURE) != NULL)
						{
							tmp.at(i)->getParentHierarchy()->removeObject(tmp.at(i));
							((Cash*)this->proctor->getParentHierarchy()->getObjectsInRadiusOf(this->proctor, radius)->getComponentOfType(GameLogic::C_CASH))->setFollow(true);
						}
						if (tmp.at(i)->getComponentOfType(C_INTERACTABLE) != NULL && tmp.at(i)->getComponentOfType(C_INTERACTABLE)->isActive() == true)
						{
							if (tmp.at(i)->name == "x-1" || tmp.at(i)->name == "x-2" || tmp.at(i)->name == "x-3" || tmp.at(i)->name == "x-4" || tmp.at(i)->name == "x-5")
							{
								tmp.at(i)->getComponentOfType(C_INTERACTABLE)->setActive(false);
								tmp.at(i)->getParentHierarchy()->removeObject(tmp.at(i));
								if (tmp.at(i)->name == "x-1")
									clueText = "The winds blow NORTH";
								if (tmp.at(i)->name == "x-2")
									clueText = "The winds blow NORTH-WEST";
								if (tmp.at(i)->name == "x-3")
									clueText = "The winds blow SOUTH-WEST";
								if (tmp.at(i)->name == "x-4")
									clueText = "The winds blow SOUTH";
								if (tmp.at(i)->name == "x-5")
									clueText = "Familiar odor...?";

								isCluePickedUp = true;
							}
						}

					}
				}
			}
			if (keyboard->isKeyPressed(GLFW_KEY_B) && isPlayerOneInBoat)
			{
				boat->setIsPlayerOneInBoat(false);
				boat->deatachPlayerOne(proctor);
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
					keyboard->isKeyDown(GLFW_KEY_L))
					&& frameCounter % 10 == 0)
				{
					if (randomNumber == 0)
						playerSounds->play2D("assets/audio/sounds/footstep_00.ogg", false);
					else if (randomNumber == 1)
						playerSounds->play2D("assets/audio/sounds/footstep_01.ogg", false);
					else if (randomNumber == 2)
						playerSounds->play2D("assets/audio/sounds/footstep_02.ogg", false);
					else if (randomNumber == 3)
						playerSounds->play2D("assets/audio/sounds/footstep_03.ogg", false);

					//((Anima*)proctor->getComponentOfType(C_ANIMA))->playAnimation(1);
				}

				/* Horizontal movement */
				if (keyboard->isKeyDown(GLFW_KEY_J))
				{
					transform.position += glm::vec3(-movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(269.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(89.5));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_L))
				{
					transform.position += glm::vec3(movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
					transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(89.5f), 0.0f);
					//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-89.5f));
				}

				/* Vertical movement */
				if (keyboard->isKeyDown(GLFW_KEY_I))
				{
					transform.position += glm::vec3(0.0f, 0.0f, -movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_J))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-134.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(134.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_L))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(134.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(224.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(179.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(179.5f));
				}
				else if (keyboard->isKeyDown(GLFW_KEY_K))
				{
					transform.position += glm::vec3(0.0f, 0.0f, movementSpeed) * deltaTime * 20.0f;

					/* Check for diagonal movement */
					if (keyboard->isKeyDown(GLFW_KEY_J))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(44.5f));

					else if (keyboard->isKeyDown(GLFW_KEY_L))
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(44.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, glm::radians(-44.5f));

					else
						transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(0.5f), 0.0f);
						//transform.rotation = glm::quat(1.0f, glm::radians(89.5f), 0.0f, 0.0f);
				}
			}

			/* Collect players update */
			if (keyboard->isKeyReleased(GLFW_KEY_PERIOD))
			{
				/* Create tmp for easy acces */
				std::vector<Proctor*> tmp = proctor->getParentHierarchy()->getInteractable();

				/* Check if player is near the interactable proctor */
				for (unsigned int i = 0; i < tmp.size(); i++)
				{
					float xDistance = tmp.at(i)->getPosition()[0] - proctor->getPosition()[0];
					float zDistance = tmp.at(i)->getPosition()[2] - proctor->getPosition()[2];
					float distance = sqrt(xDistance * xDistance + zDistance * zDistance);
					//std::cout << tmp.at(i)->name << ": " << distance << std::endl;

					if (distance <= maxInteractionDistance)
					{
						if (tmp.at(i)->name == "boat")
						{
							if (!isPlayerTwoInBoat)
							{
								boat->attachPlayerTwo(proctor);
								boat->setIsPlayerTwoInBoat(true);
							}
						}

						if (tmp.at(i)->getComponentOfType(C_TREASURE) != NULL)
						{
							tmp.at(i)->getParentHierarchy()->removeObject(tmp.at(i));
							((Cash*)this->proctor->getParentHierarchy()->getObjectsInRadiusOf(this->proctor, radius)->getComponentOfType(GameLogic::C_CASH))->setFollow(true);
						}
					}
				}
			}

			if (keyboard->isKeyPressed(GLFW_KEY_SLASH) && isPlayerTwoInBoat)
			{
				boat->setIsPlayerTwoInBoat(false);
				boat->deatachPlayerTwo(proctor);
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
}