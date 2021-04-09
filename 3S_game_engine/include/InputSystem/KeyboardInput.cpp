#include "KeyboardInput.h"
#include <algorithm>

namespace InputSystem
{
	const int KeyboardInput::NUMBER_OF_KEYS = 348;

	bool KeyboardInput::activeKeys[KeyboardInput::NUMBER_OF_KEYS];
	int KeyboardInput::keyStates[KeyboardInput::NUMBER_OF_KEYS];

	KeyboardInput::KeyboardInput(GLFWwindow* window)
	{
		this->window = window;
		update();
		glfwSetKeyCallback(window, keyCallback);
	}

	/* Reset key states after each loop */
	void KeyboardInput::update()
	{
		std::fill_n(keyStates, NUMBER_OF_KEYS, -1); //fill array with -1
	}

	/* Check if key is continuously pressed down */
	bool KeyboardInput::isKeyDown(int key) { return activeKeys[key]; }

	/* Check if key is pressed */
	bool KeyboardInput::isKeyPressed(int key) { return keyStates[key] == GLFW_PRESS; }

	/* Check if key is released */
	bool KeyboardInput::isKeyReleased(int key) { return keyStates[key] == GLFW_RELEASE; }

	/* Callback, get key states and save to arrays */
	void KeyboardInput::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		activeKeys[key] = action != GLFW_RELEASE;
		keyStates[key] = action;
	}
}