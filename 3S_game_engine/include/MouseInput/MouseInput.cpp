#include "MouseInput.h"

double MouseInput::yScroll;

MouseInput::MouseInput(GLFWwindow* window)
{
	this->window = window;	
}

MousePosition MouseInput::getCursorPosition() //return mouse position as struct with members: X,Y 
{
	glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
	return mousePosition;	
}

double MouseInput::getScrollValue() // retrun scroll value
{
	glfwSetScrollCallback(window, scroll_callback);
	return yScroll;
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

void MouseInput::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	yScroll += yoffset;
}
