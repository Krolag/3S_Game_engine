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
		useBoatInput();
		changePosition();
	}

	void Boat::attachPlayerOne(GameLogic::Proctor* player)
	{
		player->setPosition(glm::vec3(0.56f, 2.45f, 1.72f)); //position player on boat
		player->setRotation(glm::quat(1.0f, 1.57f, 0.0f, -1.57f));
		proctor->addChild(player);
		proctor->getParentHierarchy()->removeObject(player);
	}

	void Boat::deatachPlayerOne(GameLogic::Proctor* player)
	{
		proctor->removeChild(player);
		proctor->getParentHierarchy()->addObject(player);
		player->setPosition(transform.position + glm::vec3(1.72f, 2.45f, 0.0f));
		player->setRotation(glm::quat(1.0f, 1.57f, 0.0f, 3.14f));
	}

	void Boat::attachPlayerTwo(GameLogic::Proctor* player)
	{
		player->setPosition(glm::vec3(0.56f, 2.45f, -1.96f)); //position player on boat
		player->setRotation(glm::quat(1.0f, 1.57f, 0.0f, -1.57f));
		proctor->addChild(player);
		proctor->getParentHierarchy()->removeObject(player);
	}

	void Boat::deatachPlayerTwo(GameLogic::Proctor* player)
	{
		proctor->removeChild(player);
		proctor->getParentHierarchy()->addObject(player);
		player->setPosition(transform.position + glm::vec3(-1.96f, 2.45f, 0.0f));
		player->setRotation(glm::quat(1.0f, 1.57f, 0.0f, 3.14f));
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

		if (isFirstInBoat && isSecondInBoat) {

			if (keyboard->isKeyDown(GLFW_KEY_W) && keyboard->isKeyDown(GLFW_KEY_I))
			{
				speed = startSpeed;
			}
			else if (keyboard->isKeyDown(GLFW_KEY_S) && keyboard->isKeyDown(GLFW_KEY_K))
			{
				speed = -startSpeed;
			}
			else {
				if (keyboard->isKeyPressed(GLFW_KEY_W))
				{
					speed = startSpeed;
					turnSpeed = -startTurningSpeed;
				}
				else if (keyboard->isKeyPressed(GLFW_KEY_S))
				{
					speed = -startSpeed;
					turnSpeed = startTurningSpeed;
				}
				if (keyboard->isKeyPressed(GLFW_KEY_I))
				{
					speed = startSpeed;
					turnSpeed = startTurningSpeed;
				}
				else if (keyboard->isKeyPressed(GLFW_KEY_K))
				{
					speed = -startSpeed;
					turnSpeed = -startTurningSpeed;
				}
			}
		}

		if (speed > 0)
		{
			speed -= 1.5 * parent->getDeltaTime();
		}
		if (speed < 0)
		{
			speed += 1.5 * parent->getDeltaTime();
		}
		if (turnSpeed > 0)
		{
			turnSpeed -= 1.2 * parent->getDeltaTime();
		}
		if (turnSpeed < 0)
		{
			turnSpeed += 1.2 * parent->getDeltaTime();
		}
	}

	void Boat::changePosition()
	{
		transform.rotation.y += turnSpeed * proctor->getDeltaTime();
		float distance = speed * proctor->getDeltaTime();
		float distance_x = (float)(distance * cos((transform.rotation.y)));
		float distance_z = (float)(distance * -sin((transform.rotation.y)));
		transform.position.x += distance_x;
		transform.position.z += distance_z;

		time += proctor->getDeltaTime();
		double rotation = 0.001 * (sin(time * 4.1)); // simulate waves
		transform.rotation.x += rotation;

		proctor->setTransform(transform);
		((GameLogic::MeshRenderer*)proctor->getComponentOfType(GameLogic::C_MESH))->model->position = transform.position;
		((GameLogic::MeshRenderer*)proctor->getComponentOfType(GameLogic::C_MESH))->model->rotation = transform.rotation;
		((GameLogic::MeshRenderer*)proctor->getComponentOfType(GameLogic::C_MESH))->model->scale = transform.scale;
	}
}