#include "Boat.h"
#include <iostream>
#include <math.h>
#include "InputSystem/KeyboardInput.h"

namespace GameLogic
{
	Boat::Boat(GameLogic::ComponentType _type, GameLogic::Proctor* _proctor) : Component(_type, _proctor), parent(_proctor)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}

	void Boat::update()
	{
		deltaTime = parent->getParentHierarchy()->getDeltaTime();
		useBoatInput();
		changePosition();

		if (speed > 0)
		{
			speed -= 0.6f * speed * deltaTime;
		}
		if (speed < 0)
		{
			speed += 0.6f * speed * -deltaTime;
		}
		if (turnSpeed > 0)
		{
			turnSpeed -= turnSpeed * deltaTime;
		}
		if (turnSpeed < 0)
		{
			turnSpeed += turnSpeed * -deltaTime;
		}
	}

	void Boat::attachPlayerOne(GameLogic::Proctor* player)
	{
		player->setPosition(glm::vec3(0.56f, 2.45f, 1.72f)); //position player on boat
		player->setRotation(glm::quat(1.0f, 0.0f, 1.57f, 0.0f));
		proctor->addChild(player);
		proctor->getParentHierarchy()->removeObject(player);
		setIsPlayerOneInBoat(true);
	}

	void Boat::detachPlayerOne(GameLogic::Proctor* player)
	{
		proctor->removeChild(player);
		proctor->getParentHierarchy()->addObject(player);
		player->setPosition(transform.position + glm::vec3(1.72f, 2.45f, 0.0f));
		player->setRotation(glm::quat(1.0f, 3.14f, 0.0f, 3.14f));
		setIsPlayerOneInBoat(false);
	}

	void Boat::attachPlayerTwo(GameLogic::Proctor* player)
	{
		player->setPosition(glm::vec3(0.56f, 2.45f, -1.96f)); //position player on boat
		player->setRotation(glm::quat(1.0f, 0.0f, 1.57f, 0.0f));
		proctor->addChild(player);
		proctor->getParentHierarchy()->removeObject(player);
		setIsPlayerTwoInBoat(true);
	}

	void Boat::detachPlayerTwo(GameLogic::Proctor* player)
	{
		proctor->removeChild(player);
		proctor->getParentHierarchy()->addObject(player);
		player->setPosition(transform.position + glm::vec3(-1.96f, 2.45f, 0.0f));
		player->setRotation(glm::quat(1.0f, 3.14f, 0.0f, 3.14f));
		setIsPlayerTwoInBoat(false);
	}

	bool Boat::isPlayerOneInBoat()
	{
		return isFirstInBoat;
	}

	bool Boat::isPlayerTwoInBoat()
	{
		return isSecondInBoat;
	}

	void Boat::setIsPlayerOneInBoat(bool isFirstInBoat)
	{
		this->isFirstInBoat = isFirstInBoat;
	}

	void Boat::setIsPlayerTwoInBoat(bool isSecondInBoat)
	{
		this->isSecondInBoat = isSecondInBoat;
	}

	void Boat::useBoatInput()
	{
		transform = proctor->getTransform();

		if (isFirstInBoat && isSecondInBoat) 
		{
			((BoxCollider*)proctor->getComponentOfType(GameLogic::C_COLLIDER))->isStatic = false;

			if (keyboard->isKeyDown(GLFW_KEY_W) && keyboard->isKeyPressed(GLFW_KEY_V))
			{					
				if (speed < 20 * startSpeed) speed += startSpeed;
				rightTurn = false;
				leftTurn = true;				
			}
			else if (keyboard->isKeyDown(GLFW_KEY_S) && keyboard->isKeyPressed(GLFW_KEY_V))
			{
				if (speed < 20 * startSpeed) speed += -startSpeed;
				leftTurn = false;
				rightTurn = true;
			}
			if (keyboard->isKeyDown(GLFW_KEY_I) && keyboard->isKeyPressed(GLFW_KEY_PERIOD))
			{
				if (speed < 20 * startSpeed) speed += startSpeed;
				leftTurn = false;
				rightTurn = true;
			}
			else if (keyboard->isKeyDown(GLFW_KEY_K) && keyboard->isKeyPressed(GLFW_KEY_PERIOD))
			{
				if (speed < 20 * startSpeed) speed += -startSpeed;
				rightTurn = false;
				leftTurn = true;
			}


			if (rightTurn) 
			{
				if (turnSpeed > -maxTurningSpeed) {
					turnSpeed += 2 * -deltaTime;
				}
				else {
					rightTurn = false;
				}				
			}
			if (leftTurn)
			{
				if (turnSpeed < maxTurningSpeed) {
					turnSpeed += 2 * deltaTime;
				}
				else {
					leftTurn = false;
				}
			}

		}
		else 
		{
			((BoxCollider*) proctor->getComponentOfType(GameLogic::C_COLLIDER))->isStatic=true;
		}

	}

	void Boat::changePosition()
	{

		transform.rotation.y += turnSpeed * deltaTime;
		float distance = speed * proctor->getDeltaTime();
		float distance_x = (float)(distance * cos((transform.rotation.y)));
		float distance_z = (float)(distance * -sin((transform.rotation.y)));
		transform.position.x += distance_x;
		transform.position.z += distance_z;

		time += deltaTime;
		double rotation = waveRotation * (sin(time * 4.1)); // simulate waves
		transform.rotation.x += rotation;

		if (transform.position.y > 2.2f) transform.position.y -= 0.2f;
		if (transform.position.y < 2.1f) transform.position.y += 0.1f;

		proctor->setTransform(transform);

	}
}