#include "Window.h"

namespace Application
{
	Window::Window(std::string _name, float _width, float _height, bool _isFullScreen)
		: name(_name), isFullScreen(_isFullScreen)
	{
		initialize();

		/* Create window */
		if (isFullScreen)
		{
			this->window = glfwCreateWindow(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, 
				_name.c_str(), glfwGetPrimaryMonitor(), NULL);
		}
		else
		{
			this->window = glfwCreateWindow(_width, _height, _name.c_str(), NULL, NULL);
		}

		/* Check if window was created properly */
		if (this->window == NULL)
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
	}
		
	void Window::initialize()
	{
		/* Initialize and configure GLFW */
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	void Window::update()
	{
		/* Per-frame time logic */
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}
}