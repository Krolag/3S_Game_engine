#include "Importer.h"

namespace Loader
{	
	Importer::Importer(const std::string xmlPath, Shader* _model3DShader, bool _isFrom3SE, float _divider)
		: model3DShader(_model3DShader), scaleFactor(_divider), isFrom3SE(_isFrom3SE)
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
			std::cout << e.what() << std::endl;
		}

		// Get the root node
		xml_node <>* rootNode = sceneDocument.first_node();

		// Get the first node of the scene
		rootNode = rootNode->first_node()->first_node();

		if (_isFrom3SE == true)
			_divider = 1.0f;
		// Process scene nodes and init proctors
		processElements(rootNode, _divider, _isFrom3SE);
	}
	
	void Importer::processElements(xml_node<>* firstNode, float _divider, bool _isFrom3SE, bool ifProcessingChild)
	{
		// Iterate over the elements
		for(firstNode;firstNode;firstNode=firstNode->next_sibling())
		{
			std::string proctorName = firstNode->first_attribute("name")->value();
			std::string modelName = prepareModelName(proctorName);	// just proctor name without number
			// Make a path with accurate extension
			std::string modelPath = "assets/models/serializable/" + modelName + ".fbx";
			
			// Add model to the model library if it is not already on the list
			if (!isNameInImportedList(modelName))
			{
				importedModelLibrary.addModel(modelPath, modelName, true, true);
			}

			xml_node<>* childElement;

			// Rotation
			glm::quat proctorRotation;
			childElement = firstNode->first_node("Rotation");
			if (isFrom3SE)
			{
				proctorRotation = glm::quat(
					(float)strtod(childElement->first_node("w")->value(), NULL),
					(float)strtod(childElement->first_node("x")->value(), NULL),
					(float)strtod(childElement->first_node("y")->value(), NULL),
					(float)strtod(childElement->first_node("z")->value(), NULL)
				);
			}
			else
			{
				childElement = childElement->first_node("eulerAngles");
				proctorRotation = glm::quat(
					1.0f,
					0.0f,
					-glm::radians((float)strtod(childElement->first_node("y")->value(), NULL)) - glm::radians(180.0f),
					0.0f
				);
			}

			// Scale
			childElement = firstNode->first_node("Scale");
			//glm::vec3 proctorScale(10.0f);
			glm::vec3 proctorScale(
				(float)strtod(childElement->first_node("x")->value(), NULL) / _divider, //0.05f,
				(float)strtod(childElement->first_node("y")->value(), NULL) / _divider, //0.05f,
				(float)strtod(childElement->first_node("z")->value(), NULL) / _divider //0.05f
			);

			// Position
			childElement = firstNode->first_node("Position");
			glm::vec3 proctorPosition;
			if (_isFrom3SE == false)
			{
				proctorPosition = glm::vec3(
					(float)strtod(childElement->first_node("x")->value(), NULL) * 10,//* (proctorScale.x * 100.0f),
					(float)strtod(childElement->first_node("y")->value(), NULL) * 10,//* (proctorScale.y * 100.0f),
					-(float)strtod(childElement->first_node("z")->value(), NULL) * 10//* (proctorScale.z * 100.0f)
				);
			}
			else if (_isFrom3SE == true)
			{
				proctorPosition = glm::vec3(
					(float)strtod(childElement->first_node("x")->value(), NULL), //* (proctorScale.x * 10),
					(float)strtod(childElement->first_node("y")->value(), NULL),// * (proctorScale.y * 10),
					(float)strtod(childElement->first_node("z")->value(), NULL)// * (proctorScale.z * 10)
				);
			}

			importedProctors.push_back(std::make_shared<GameLogic::Proctor>(proctorName.c_str(), proctorPosition, proctorRotation, proctorScale));

			// Components
			childElement = firstNode->first_node("components");

			std::string componentsFromXML[5]{
				childElement->first_node("isColliderOn")->value(),
				childElement->first_node("isColliderStatic")->value(),
				childElement->first_node("isInteractable")->value(),
				childElement->first_node("isTreasure")->value(),
				childElement->first_node("isCash")->value()
			};

			//Setting correct bool values
			std::vector<bool> compo;

			for (int i = 0; i < 5; i++)
			{
				if (componentsFromXML[i] == "true")
					compo.push_back(true);
				else
					compo.push_back(false);
			}
			componetsBooleanValues.push_back(compo);
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