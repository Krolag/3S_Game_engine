#ifndef HEADER_H
#define HEADER_H
#include "Shader/Shader.h"
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

	bool isPressed(int button);

	bool isReleased(int button);



private:
	GLFWwindow* window;
};
#endif



