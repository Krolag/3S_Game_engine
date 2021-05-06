#include "Importer.h"
#include <iostream>
#include <fstream>

#include "RapidXml/rapidxml_print.hpp"
#include "RapidXml/rapidxml_utils.hpp"

namespace Loader
{	
	Importer::Importer(const std::string xmlPath, Shader* _model3DShader) : model3DShader(_model3DShader)
	{
		// Get xml document to work with
		auto xmlFileContext = file2char(xmlPath.c_str());

		xml_document <> sceneDocument;
		try
		{
			sceneDocument.parse < 0 >(xmlFileContext.get()); // (2)
		}
		catch (const parse_error& e)
		{
			std::cerr << e.what() << " here: " << e.where < char >() << std::endl;
		}

		// Get the root node
		xml_node <>* rootNode = sceneDocument.first_node();

		// Get the first node of the scene
		rootNode = rootNode->first_node()->first_node();

		// Process scene nodes and init proctors
		processElements(rootNode);
	}
	
	void Importer::processElements(xml_node<>* firstNode, bool ifProcessingChild)
	{
		// Iterate over the elements
		for(firstNode;firstNode;firstNode=firstNode->next_sibling())
		{
			std::string proctorName = firstNode->first_attribute("name")->value();
			std::string modelName = prepareModelName(proctorName);	// just proctor name without number

			// Make a path with accurate extension
			std::string modelPath = "assets/models/task_models_update/" + modelName + ".fbx";
			// TODO: ask if xml file shouldn't include model extension type and process elements shouldn't tak it into account
			
			// Add model to the model library if it is not already on the list
			if (!isNameInImportedList(modelName))
			{
				importedModelLibrary.addModel(modelPath, modelName, true);
			}

			// no need for this probably
			// model id
			//int modelID = atoi(firstNode->first_attribute("model_id")->value());
			//std::cout << "MODEL ID VALUE CHECK: " << modelID << std::endl;
			
			// Position
			xml_node<>* childElement = firstNode->first_node("Position");
			glm::vec3 proctorPosition(
				(float)strtod(childElement->first_node("x")->value(), NULL),
				(float)strtod(childElement->first_node("y")->value(), NULL),
				(float)strtod(childElement->first_node("z")->value(), NULL)
			);

			// TODO: quats x,y,z,w or angles x,y,z?
		
			// Rotation
			//childElement = firstNode->first_node("Rotation");
			//glm::quat proctorRotation(
			//	(float)strtod(childElement->first_node("w")->value(), NULL),
			//	(float)strtod(childElement->first_node("x")->value(), NULL),
			//	(float)strtod(childElement->first_node("y")->value(), NULL),
			//	(float)strtod(childElement->first_node("z")->value(), NULL)
			//);

			// Scale
			childElement = firstNode->first_node("Scale");
			glm::vec3 proctorScale(
				(float)strtod(childElement->first_node("x")->value(), NULL),
				(float)strtod(childElement->first_node("y")->value(), NULL),
				(float)strtod(childElement->first_node("z")->value(), NULL)
			);

			importedProctors.push_back(std::make_shared<GameLogic::Proctor>(proctorName.c_str(), proctorPosition, glm::quat(1.0f, glm::vec3(0.0f)), proctorScale));
		}
	}

	std::unique_ptr<char[]> Importer::file2char(const char* fileName)
	{
		std::ifstream file(fileName);
		if (!file)
		{
			throw std::runtime_error("File '" + std::string(fileName) + " can't be opened!");
		}
		file.seekg(0, file.end);
		const auto fileSize = file.tellg();
		file.seekg(0);

		std::unique_ptr < char[] > fileContext(new char[fileSize + static_cast <decltype(fileSize)>(1)]);
		file.read(fileContext.get(), fileSize);

		return fileContext;
	}

	std::string Importer::prepareModelName(std::string name)
	{
		std::string retStr = "";
		for(char c : name)
		{
			if (c == '-') break;
			retStr += c;
		}
		return retStr;
	}

	bool Importer::isNameInImportedList(std::string name)
	{
		
		for (int i = 0; i < importedModelLibrary.loadedModels.size(); ++i)
		{
			if(importedModelLibrary.loadedModels.at(i).name == name)
			{
				return true;
			}
		}
		return false;
	}
}