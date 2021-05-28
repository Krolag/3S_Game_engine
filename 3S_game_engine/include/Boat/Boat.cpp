#include "Boat.h"
#include <iostream>
#include <math.h>

Boat::Boat(GameLogic::ComponentType _type, GameLogic::Proctor* _proctor): Component(_type, _proctor), parent(_proctor)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}


void Boat::update()
{

	useBoatInput();
	transform.rotation.y += turnSpeed * proctor->getDeltaTime();
	float distance = speed * proctor->getDeltaTime();
	float dx = (float)(distance * cos((transform.rotation.y)));
	float dz = (float)(distance * -sin((transform.rotation.y)));
	//std::cout << speed << "\n";
	transform.position.x += dx;
	transform.position.z += dz;


	proctor->setTransform(transform);
	((GameLogic::MeshRenderer*)proctor->getComponentOfType(GameLogic::C_MESH))->model->position = transform.position;
	((GameLogic::MeshRenderer*)proctor->getComponentOfType(GameLogic::C_MESH))->model->rotation = transform.rotation;
	((GameLogic::MeshRenderer*)proctor->getComponentOfType(GameLogic::C_MESH))->model->scale = transform.scale;

}

void Boat::useBoatInput()
{
	transform = proctor->getTransform();

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
		else if(keyboard->isKeyPressed(GLFW_KEY_S))
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
		turnSpeed -= 1 * parent->getDeltaTime();
	}
	if (turnSpeed < 0)
	{
		turnSpeed += 1 * parent->getDeltaTime();
	}
}
