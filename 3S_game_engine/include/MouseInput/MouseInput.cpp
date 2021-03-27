#include "MouseInput.h"


MouseInput::MouseInput(GLFWwindow* window)
{
	this->window = window;	
}

MousePosition MouseInput::getCursorPosition() //return mouse position as struct with members: X,Y 
{
	glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
	return mousePosition;	
}

bool MouseInput::isPressed(int button) // check if button is pressed
{
	int state = glfwGetMouseButton(window, button);
	if (state == GLFW_PRESS)
	{
		return true;
	}
	return false;
}

bool MouseInput::isReleased(int button) //check if button is released
{
	int state = glfwGetMouseButton(window, button);
	if (state == GLFW_RELEASE)
	{
		return true;
	}
	return false;
}
