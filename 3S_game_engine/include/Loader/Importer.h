#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include "ModelLibrary.h"
#include "GameLogic/Proctor.h"
#include "GameLogic/GameLogic.h"
#include "RapidXml/rapidxml.hpp"
#include "RapidXml/rapidxml_print.hpp"
#include "RapidXml/rapidxml_utils.hpp"

namespace Loader
{
#define M_PI 3.14159265359
	using namespace rapidxml;
	
	class Importer
	{
	public:
		/* Public fields */
		Loader::ModelLibrary importedModelLibrary;
		std::vector<std::shared_ptr<GameLogic::Proctor>> importedProctors;
		std::vector<std::vector<bool>> componetsBooleanValues;
		std::vector<std::shared_ptr<GameLogic::MeshRenderer>> meshRenderers;
		std::vector<std::shared_ptr<GameLogic::BoxCollider>> boxColliders;
		std::vector<std::shared_ptr<GameLogic::Interactable>> interactables;
		std::vector<std::shared_ptr<GameLogic::Treasure>> treasures;
		std::vector<std::shared_ptr<GameLogic::Cash>> cash;
		std::vector<std::shared_ptr<GameLogic::Enemy>> enemies;
		
		/* Constructor */
		Importer(const std::string xmlPath, Shader* _model3DShader, bool _isFrom3SE, float _divider);

		std::string prepareModelName(std::string name);

	private:
		Shader* model3DShader;
		float scaleFactor;
		bool isFrom3SE;
		
		/* Importer methods used for load */
		void processElements(xml_node<>* firstNode, float _divider, bool _isFrom3SE, bool ifProcessingChild = false);
		std::unique_ptr<char[]> file2char(const char* fileName);
		bool isNameInImportedList(std::string name);
	};
}
