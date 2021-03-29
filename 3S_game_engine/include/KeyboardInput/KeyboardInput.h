#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H
#include <GLFW/glfw3.h>

class KeyboardInput
{
public:

	KeyboardInput(GLFWwindow* window);

	bool isPressed(int button);

	bool isReleased(int button);

private:

	GLFWwindow* window;

};
#endif
