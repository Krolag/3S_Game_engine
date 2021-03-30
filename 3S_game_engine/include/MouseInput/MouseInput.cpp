#include "MouseInput.h"
#include <algorithm>

const int MouseInput::NUMBER_OF_BUTTONS = 8;

bool MouseInput::activeButtons[MouseInput::NUMBER_OF_BUTTONS];
int MouseInput::buttonStates[MouseInput::NUMBER_OF_BUTTONS];

int MouseInput::entered;
double MouseInput::yScroll;


MouseInput::MouseInput(GLFWwindow* window)
{
	this->window = window;	
	glfwSetCursorEnterCallback(window, cursorEnterCallback);
	glfwSetMouseButtonCallback(window, buttonCallback);
}

void MouseInput::cursorEnable()		//enable cursor in window
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void MouseInput::cursorDisable() //disable cursor in window
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool MouseInput::isButtonDown(int button) // check if button is continuously pressed down
{
	return activeButtons[button];
}

bool MouseInput::isButtonPressed(int button) //check if button is pressed
{
	return buttonStates[button] == GLFW_PRESS;
}

bool MouseInput::isButtonReleased(int button) //check if button is relesed
{
	return buttonStates[button] == GLFW_RELEASE;
}

void MouseInput::update() //reset button states
{
	std::fill_n(buttonStates, NUMBER_OF_BUTTONS, -1);
}

MousePosition MouseInput::getCursorPosition() //return mouse position as struct with members: X,Y 
{

	glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
	return mousePosition;	
}

double MouseInput::getScrollValue() // retrun scroll value
{
	glfwSetScrollCallback(window, scrollCallback);
	return yScroll;
}

void MouseInput::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
{
	yScroll += yoffset;
}

void MouseInput::cursorEnterCallback(GLFWwindow* window, int entered)
{
	MouseInput::entered = entered;
}

void MouseInput::buttonCallback(GLFWwindow* window, int button, int action, int mods)
{
	activeButtons[button] = action != GLFW_RELEASE;
	buttonStates[button] = action;
}
