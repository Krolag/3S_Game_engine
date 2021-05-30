#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace Application
{
	class Window
	{
	public:
		GLFWwindow* window;
		float deltaTime = 0.0f;

		/* Constructors */
		Window(std::string _name, float _width = 1280, float _height = 720, bool _isFullScreen = false);
		
		void update();

	private:
		bool isFullScreen;
		float lastFrame = 0.0f;
		std::string name;

		void initialize();
	};
}

#endif // !WINDOW_H