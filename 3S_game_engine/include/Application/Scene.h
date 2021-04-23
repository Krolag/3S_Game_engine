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
		float deltaTime = 0.0f;

		/* Constructors */
		Scene(std::string _name, float _width = 1280, float _height = 720, bool _isFullScreen = false);
		
		void update();

	private:
		bool isFullScreen;
		float lastFrame = 0.0f;
		std::string name;

		void initialize();
	};
}

#endif // !SCENE_H