#ifndef HEADER_H
#define HEADER_H

#include <GLFW/glfw3.h>

struct MousePosition
{
	double x, y;
};

class MouseInput{
	
public:
	
	MousePosition mousePosition;

	MouseInput(GLFWwindow* window);

	MousePosition getCursorPosition();

	bool isCursorEntered();

	void cursorEnable();

	void cursorDisable();

	bool isButtonDown(int button);

	bool isButtonPressed(int button);

	bool isButtonReleased(int button);

	void update();

	double getScrollValue();


private:

	static const int NUMBER_OF_BUTTONS;

	GLFWwindow* window;

	static int entered;

	static int buttonStates[];

	static bool activeButtons[];

	static double yScroll;

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	static void cursorEnterCallback(GLFWwindow* window, int entered);

	static void buttonCallback(GLFWwindow* window, int button, int action, int mods);

};
#endif



