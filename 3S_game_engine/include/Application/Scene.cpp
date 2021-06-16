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
		cActiveScene["enterStory_00"] = false;
		cActiveScene["enterStory_01"] = false;
		cActiveScene["tutorial"] = false;
		cActiveScene["options"] = false;
		cActiveScene["game"] = false;
		cActiveScene["resume"] = false;
		cActiveScene["exitStory_00"] = false;
		cActiveScene["exitStory_01"] = false;
		cActiveScene["credits"] = false;

		changeCurrentScene("mainMenu");
	}
}