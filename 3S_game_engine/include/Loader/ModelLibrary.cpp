#include "ModelLibrary.h"

namespace Loader
{
	void ModelLibrary::addModel(std::string _path, std::string _name, bool _noTex)
	{
		for (unsigned int i = 0; i < loadedModels.size(); i++)
		{
			if (loadedModels[i].path == _path)
			{
				loadedModels.push_back(loadedModels[i]);
				loadedModels.back().name = _name;
				return;
			}
		}

		loadedModels.push_back(Model(_path, _name, _noTex));
	}

	Model* ModelLibrary::getModel(std::string _name)
	{
		for (unsigned int i = 0; i < loadedModels.size(); i++)
		{
			if (loadedModels[i].name == _name)
			{
				return &loadedModels[i];
			}
		}
		
		return NULL;
	}
}


