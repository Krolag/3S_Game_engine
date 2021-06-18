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
	constexpr float ATTACK_RADIUS = 6.3f;
	constexpr float DAMAGE_RADIUS = 8.3f;
	constexpr float ATTACK_TIME = 1.0f;
	constexpr float PLAYER_DAMAGE = 51.0f;
	
	PlayerInput::PlayerInput(ComponentType _type, Proctor* _proctor)
		: Component(_type, _proctor), parent(_proctor), attackTimer(0.0f)
	{ 
		if (_proctor != NULL)
		{
			srand(time(NULL));
			proctor->addComponent(this);
		}
	}

	PlayerInput::PlayerInput(ComponentType _type, Proctor* _proctor, bool _isPlayerOne, Boat* _boat)
		: Component(_type, _proctor), parent(_proctor), isPlayerOne(_isPlayerOne), boat(_boat), attackTimer(0.0f)
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

		Proctor* nearestEnemy = getNearestEnemy();
		/* Check if nearest enemy is within attack radius */
		if(distanceTo(nearestEnemy) < ATTACK_RADIUS || nearestEnemy != NULL)
		{
			std::cout << "enemy is within attack radius of " << proctor->name << ", distance = " << distanceTo(nearestEnemy) << "\n";
			playerInAttackRadius = true;
			///* Calculate direction to nearest enemy */
			//glm::vec3 enemyPos = nearestEnemy->getPosition();
			//glm::vec3 playerPos = proctor->getPosition();
			//glm::vec3 direction = glm::normalize(playerPos - enemyPos);
			//glm::vec3 playerNormalDown = glm::vec3(0.0f, 0.0f, -1.0f);

			///* Calc rotation to look at the chased player */
			//float dot = playerNormalDown.x * direction.x + playerNormalDown.z * direction.z;	// dot product between enemy starting rotation and direction to player
			//float det = playerNormalDown.x * direction.z - direction.x * playerNormalDown.z;	// determinant
			//float angle = atan2(det, dot);												// angle between enemy starting position and direction to player in radians

			///* Apply rotation */
			//glm::quat enemyStartRotation = glm::quat(1.0f, 0.0f, -angle, 0.0f);
			//proctor->setRotation(enemyStartRotation);
			
			/* Show possibility of attack in controls if not attacking */
			// TODO: @Anyone show possibility of attack for player and change boolean isPlayerAttacking
			
			/* If attacking then check for time on timer */
			if(isPlayerAttacking)
			{
				if (attackTimer > ATTACK_TIME)
				{
					/* Attack enemy and apply damage to it if timer passed the attack time */
					if (isPlayerOne)
						playerSounds->play2D(attack_D, false);
					else
						playerSounds->play2D(attack_K, false);

					((Enemy*)nearestEnemy)->takeDamage(proctor, PLAYER_DAMAGE);
					attackTimer = 0.0f;
					isPlayerAttacking = false;
				}
				/* Increment attacking timer */
				attackTimer += proctor->getParentHierarchy()->getDeltaTime();
			}
		}
		else
		{
			playerInAttackRadius = false;
		}
		
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

		/* If any player is moving */
		if (isPlayerMoving)
		{
			if (cTimeInAir >= 0 && cTimeInAir < 0.2)
			{
				proctor->transform.position.y += 10.0f * proctor->getDeltaTime();
				isInAir = true;
			}
			if (cTimeInAir >= 0.2 && cTimeInAir < 0.4)
			{
				proctor->transform.position.y -= 10.0f * proctor->getDeltaTime();
				isInAir = false;
			}
			if (cTimeInAir >= 0.4)
				cTimeInAir = 0;
		}
		cTimeInAir += proctor->getDeltaTime();
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

	Proctor* PlayerInput::getNearestEnemy()
	{
		std::vector<Proctor*> enemiesList = proctor->getParentHierarchy()->getEnemiesList();
		float minDistance = FLT_MAX;
		Proctor* nearestEnemy = NULL;
		for (auto enemy : enemiesList)
		{
			if(distanceTo(enemy) < minDistance)
			{
				nearestEnemy = enemy;
			}
		}
		return nearestEnemy;
	}

	float PlayerInput::distanceTo(Proctor* _proctor)
	{
		return glm::distance(glm::vec2(_proctor->getPosition().x, _proctor->getPosition().z), glm::vec2(proctor->getPosition().x, proctor->getPosition().z));
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
					if (!isPlayerOneInBoat)
						isPlayerMoving = true;

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
				else
					isPlayerMoving = false;

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
			if(!isPlayerAttacking)
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
					if (!isPlayerTwoInBoat)
						isPlayerMoving = true;

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
				else
				{
					isPlayerMoving = false;
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
			if (!isPlayerAttacking)
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