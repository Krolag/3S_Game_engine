#include "KeyboardInput.h"
#include<algorithm>

bool KeyboardInput::activeKeys[KEY_NUMBERS];
int KeyboardInput::keyStates[KEY_NUMBERS];

KeyboardInput::KeyboardInput(GLFWwindow* window)
{
	this->window = window;
	update();	
	glfwSetKeyCallback(window, keyCallback);
}

void KeyboardInput::update() //reset key states after each loop
{
	std::fill_n(keyStates, 512, -1); //fill array with -1
}

bool KeyboardInput::isKeyDown(int key) // check if key is continuously pressed down
{
	return activeKeys[key];
}

bool KeyboardInput::isKeyPressed(int key) //check if key is pressed
{
	return keyStates[key] == GLFW_PRESS;
}

bool KeyboardInput::isKeyReleased(int key) //check if key is released
{
	return keyStates[key] == GLFW_RELEASE;
}

void KeyboardInput::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) // callback, get key states and save to arrays
{
	activeKeys[key] = action != GLFW_RELEASE; 
	keyStates[key] = action;
}


