#pragma once
#include "ModelLibrary.h"
#include "GameLogic/Proctor.h"
#include "RapidXml/rapidxml.hpp"

namespace Loader
{
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
		
		/* Constructor */
		Importer(const std::string xmlPath, Shader* _model3DShader, float _divider);

		/* Public methods */
		std::string prepareModelName(std::string name);
	private:
		Shader * model3DShader;
		
		/* Importer methods used for load */
		void processElements(xml_node<>* firstNode, float _divider, bool ifProcessingChild = false);
		std::unique_ptr<char[]> file2char(const char* fileName);
		bool isNameInImportedList(std::string name);
	};
}
