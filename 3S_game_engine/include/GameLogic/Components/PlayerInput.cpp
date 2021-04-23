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
			usePlayerOneInput();
		}
		else
		{
			usePlayerTwoInput();
		}
	}

	void PlayerInput::usePlayerOneInput()
	{
		/* Get deltaTime from proctor */
		float deltaTime = proctor->getDeltaTime();

		/* Get proctors parent transform */
		Transform transform = proctor->getTransform();

		/* Calculate speed */
		movementSpeed += acceleration * deltaTime;
		if (movementSpeed >= maxSpeed)
		{
			movementSpeed = maxSpeed;
		}

		/* Horizontal movement */
		if (keyboard->isKeyDown(GLFW_KEY_A))
		{
			transform.position += glm::vec3(-movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
			transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(270.0f), 0.0f);
		}
		else if (keyboard->isKeyDown(GLFW_KEY_D))
		{
			transform.position += glm::vec3(movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
			transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(90.0f), 0.0f);
		}

		/* Vertical movement */
		if (keyboard->isKeyDown(GLFW_KEY_W))
		{
			transform.position += glm::vec3(0.0f, 0.0f, -movementSpeed) * deltaTime * 20.0f;

			/* Check for diagonal movement */
			if (keyboard->isKeyDown(GLFW_KEY_A))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(225.0f), 0.0f);
			}
			else if (keyboard->isKeyDown(GLFW_KEY_D))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(135.0f), 0.0f);
			}
			else
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(180.0f), 0.0f);
			}
		}
		else if (keyboard->isKeyDown(GLFW_KEY_S))
		{
			transform.position += glm::vec3(0.0f, 0.0f, movementSpeed) * deltaTime * 20.0f;

			/* Check for diagonal movement */
			if (keyboard->isKeyDown(GLFW_KEY_A))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-45.0f), 0.0f);
			}
			else if (keyboard->isKeyDown(GLFW_KEY_D))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(45.0f), 0.0f);
			}
			else
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(0.0f), 0.0f);
			}
		}

		/* Collect players update */
		if (keyboard->isKeyDown(GLFW_KEY_V))
		{
			std::cout << "primary button" << std::endl;
		}
		if (keyboard->isKeyDown(GLFW_KEY_B))
		{
			std::cout << "secondary button" << std::endl;
		}

		/* Check if player is holding any movement button */
		if (!keyboard->isKeyDown(GLFW_KEY_W) && !keyboard->isKeyDown(GLFW_KEY_S)
			&& !keyboard->isKeyDown(GLFW_KEY_A) && !keyboard->isKeyDown(GLFW_KEY_D))
		{
			movementSpeed = 0.0f;
		}

		/* Update players transform */
		proctor->setTransform(transform);
	}

	void PlayerInput::usePlayerTwoInput()
	{
		/* Get deltaTime from proctor */
		float deltaTime = proctor->getDeltaTime();

		/* Get proctors parent transform */
		Transform transform = proctor->getTransform();

		/* Calculate speed */
		movementSpeed += acceleration * deltaTime;
		if (movementSpeed >= maxSpeed)
		{
			movementSpeed = maxSpeed;
		}

		/* Horizontal movement */
		if (keyboard->isKeyDown(GLFW_KEY_J))
		{
			transform.position += glm::vec3(-movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
			transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(270.0f), 0.0f);
		}
		else if (keyboard->isKeyDown(GLFW_KEY_L))
		{
			transform.position += glm::vec3(movementSpeed, 0.0f, 0.0f) * deltaTime * 20.0f;
			transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(90.0f), 0.0f);
		}

		/* Vertical movement */
		if (keyboard->isKeyDown(GLFW_KEY_I))
		{
			transform.position += glm::vec3(0.0f, 0.0f, -movementSpeed) * deltaTime * 20.0f;

			/* Check for diagonal movement */
			if (keyboard->isKeyDown(GLFW_KEY_J))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(225.0f), 0.0f);
			}
			else if (keyboard->isKeyDown(GLFW_KEY_L))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(135.0f), 0.0f);
			}
			else
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(180.0f), 0.0f);
			}
		}
		else if (keyboard->isKeyDown(GLFW_KEY_K))
		{
			transform.position += glm::vec3(0.0f, 0.0f, movementSpeed) * deltaTime * 20.0f;

			/* Check for diagonal movement */
			if (keyboard->isKeyDown(GLFW_KEY_J))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(-45.0f), 0.0f);
			}
			else if (keyboard->isKeyDown(GLFW_KEY_L))
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(45.0f), 0.0f);
			}
			else
			{
				transform.rotation = glm::quat(1.0f, 0.0f, glm::radians(0.0f), 0.0f);
			}
		}

		/* Collect players update */
		if (keyboard->isKeyDown(GLFW_KEY_PERIOD))
		{
			std::cout << "primary button" << std::endl;
		}
		if (keyboard->isKeyDown(GLFW_KEY_SLASH))
		{
			std::cout << "secondary button" << std::endl;
		}

		/* Check if player is holding any movement button */
		if (!keyboard->isKeyDown(GLFW_KEY_I) && !keyboard->isKeyDown(GLFW_KEY_K)
			&& !keyboard->isKeyDown(GLFW_KEY_J) && !keyboard->isKeyDown(GLFW_KEY_L))
		{
			movementSpeed = 0.0f;
		}

		/* Update players transform */
		proctor->setTransform(transform);
	}
}