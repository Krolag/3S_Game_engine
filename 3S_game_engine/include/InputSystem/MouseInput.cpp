#include "MouseInput.h"
#include <algorithm>

namespace InputSystem
{
	const int MouseInput::NUMBER_OF_BUTTONS = 8;

	bool MouseInput::activeButtons[MouseInput::NUMBER_OF_BUTTONS];
	int MouseInput::buttonStates[MouseInput::NUMBER_OF_BUTTONS];

	int MouseInput::entered;
	double MouseInput::yScroll = 45;

	MouseInput::MouseInput(GLFWwindow* window)
	{
		this->window = window;
		glfwSetCursorEnterCallback(window, cursorEnterCallback);
		glfwSetMouseButtonCallback(window, buttonCallback);
		glfwSetScrollCallback(window, scrollCallback);
	}

	/* Enable cursor in window */
	void MouseInput::cursorEnable() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

	/* Disable cursor in window */
	void MouseInput::cursorDisable() { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }

	bool MouseInput::isButtonDown(int button) { return activeButtons[button]; }

	bool MouseInput::isButtonPressed(int button) { return buttonStates[button] == GLFW_PRESS; }

	bool MouseInput::isButtonReleased(int button) { return buttonStates[button] == GLFW_RELEASE; }

	/* Reset button states */
	void MouseInput::update() { std::fill_n(buttonStates, NUMBER_OF_BUTTONS, -1); }

	/* Return mouse position as struct with members: X,Y */
	MousePosition MouseInput::getCursorPosition()
	{
		glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
		return mousePosition;
	}

	bool MouseInput::isCursorEntered() { return entered; }

	double MouseInput::getScrollValue() { return yScroll; }

	void MouseInput::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) { yScroll -= yoffset; }

	void MouseInput::cursorEnterCallback(GLFWwindow* window, int entered) { MouseInput::entered = entered; }

	void MouseInput::buttonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		activeButtons[button] = action != GLFW_RELEASE;
		buttonStates[button] = action;
	}
}