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
		std::vector<std::shared_ptr<GameLogic::MeshRenderer>> meshRenderers;
		
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
