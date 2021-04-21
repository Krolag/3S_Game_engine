#include "PlayerInput.h"

namespace GameLogic
{
	PlayerInput::PlayerInput(ComponentType _type, Proctor* _proctor)
		: Component(_type, _proctor), parent(proctor)
	{ }

	PlayerInput::PlayerInput(ComponentType _type, Proctor* _proctor, bool _isPlayerOne)
		: Component(_type, _proctor), parent(proctor), isPlayerOne(_isPlayerOne)
	{ }

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
		/* Check which player should get update */
		if (isPlayerOne) 
		{
			usePlayerOneMovement();
		}
		else
		{

		}
	}


	void PlayerInput::usePlayerOneMovement()
	{
		/* Get proctors parent transform */
		Transform transform = proctor->getTransform();

		if (keyboard->isKeyDown(GLFW_KEY_W))
		{
			std::cout << "move up" << std::endl;
		}
		else if (keyboard->isKeyDown(GLFW_KEY_S))
		{
			std::cout << "move down" << std::endl;
		}
	}
}