#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>
#include <map>

#include "UIRender/UIRender.h"

namespace Application
{
	class Scene
	{
	public:
		std::map<std::string, bool> cActiveScene;
		
		Scene();
		
		void changeCurrentScene(std::string _sceneName);

	private:
		void initializeSceneMap();
	};
}

#endif // !SCENE_H