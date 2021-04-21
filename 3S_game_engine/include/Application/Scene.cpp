#include "Scene.h"

namespace Application
{
	Scene::Scene(std::string _name, float _width, float _height, bool _isFullScreen)
		: name(_name), isFullScreen(_isFullScreen)
	{
		initialize();

		/* Create window */
		if (isFullScreen)
		{
			this->window = glfwCreateWindow(glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, "3S GameEngine", glfwGetPrimaryMonitor(), NULL);
		}
		else
		{
			this->window = glfwCreateWindow(_width, _height, "3S GameEngine", NULL, NULL);
		}

		/* Check if window was created properly */
		if (this->window == NULL)
		{
			std::cerr << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
	}
		
	void Scene::initialize()
	{
		/* Initialize and configure GLFW */
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		#endif
	}
}