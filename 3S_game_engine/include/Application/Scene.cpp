#include "Scene.h"

namespace Application 
{
	Scene::Scene()
	{
		initializeSceneMap();
	}
	
	void Scene::changeCurrentScene(std::string _sceneName)
	{
		/* Loop through map values */
		for (auto it = cActiveScene.begin(); it != cActiveScene.end(); it++)
		{
			/* Check if key value is equal to true*/
			if (it->second == true)
			{
				it->second = false;
			}
		}

		/* Assign true for given name */
		cActiveScene[_sceneName] = true;
	}

	void Scene::initializeSceneMap()
	{
		cActiveScene["mainMenu"] = false;
		cActiveScene["enterStory"] = false;
		cActiveScene["tutorial"] = false;
		cActiveScene["game"] = false;
		cActiveScene["exitStory"] = false;

		changeCurrentScene("game"); // Change it later for "mainMenu"
	}
}