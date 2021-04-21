#pragma once
#ifndef SCENE_H
#define SCENE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace Application
{
	class Scene
	{
	public:
		GLFWwindow* window;

		Scene(std::string _name, float _width = 1280, float _height = 720, bool _isFullScreen = false);
	
	private:
		bool isFullScreen;
		std::string name;

		void initialize();

	};
}

#endif // !SCENE_H
