#pragma once
#ifndef MODELLIBRARY_H
#define MODELLIBRARY_H

#include "Model.h"
#include <iostream>

namespace Loader
{
	class ModelLibrary
	{
	public:
		std::vector<Model> loadedModels;

		ModelLibrary() {}

		void addModel(std::string _path, std::string _name, bool _noTex);
		void addModel(std::string _path, std::string _name, bool _noTex, bool _noAnim);
		Model* getModel(std::string _name);

	private:
	};
}

#endif // !MODELLIBRARY_H