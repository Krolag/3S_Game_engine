#include "KeyboardInput.h"

KeyboardInput::KeyboardInput(GLFWwindow* window)
{
	this->window = window;
}

bool KeyboardInput::isPressed(int button)
{
	int state = glfwGetKey(window, button);
	if (state == GLFW_PRESS)
	{
		return true;
	}
	return false;
}

bool KeyboardInput::isReleased(int button)
{
	int state = glfwGetKey(window, button);
	if (state == GLFW_RELEASE)
	{
		return true;
	}
	return false;
}

