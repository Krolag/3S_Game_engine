#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H
#include <GLFW/glfw3.h>

class KeyboardInput
{
public:

	KeyboardInput(GLFWwindow* window);

	void update();

	bool isKeyDown(int key);

	bool isKeyPressed(int key);

	bool isKeyReleased(int key);

private:

	GLFWwindow* window;

	const static int KEY_NUMBERS = 348;

	static bool activeKeys[];

	static int keyStates[];

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};
#endif
