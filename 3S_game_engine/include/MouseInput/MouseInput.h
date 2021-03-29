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

	double getScrollValue();

	bool isPressed(int button);

	bool isReleased(int button);

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	GLFWwindow* window;
	static double yScroll;
};
#endif



