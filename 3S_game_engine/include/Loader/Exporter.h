#pragma once
#include "GameLogic/GameLogic.h"
#include "Importer.h"
#include "RapidXml/rapidxml.hpp"
#include "RapidXml/rapidxml_print.hpp"
#include "RapidXml/rapidxml_utils.hpp"


namespace Loader
{
	using namespace rapidxml;

	class Exporter
	{
	public:
		static void exportScene(std::vector<GameLogic::Proctor*> _proctors, std::string _path)
		{
			xml_document<> sceneDocument;
			xml_node<>* decl = sceneDocument.allocate_node(node_declaration);
			decl->append_attribute(sceneDocument.allocate_attribute("version", "1.0"));
			sceneDocument.append_node(decl);
			xml_node<>* gameModelsCollection = sceneDocument.allocate_node(node_element, "GameModelsCollection");
			gameModelsCollection->append_attribute(sceneDocument.allocate_attribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema"));
			gameModelsCollection->append_attribute(sceneDocument.allocate_attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"));
			xml_node<>* gameModels = sceneDocument.allocate_node(node_element, "GameModels");
			for (int i = 5; i < _proctors.size(); ++i)
			{
				if (_proctors.at(i)->name.c_str() == "boat" || _proctors.at(i)->name.c_str() == "playerOne" || _proctors.at(i)->name.c_str() == "playerTwo")
					continue;
				/* New node */
				xml_node<>* gameModel = sceneDocument.allocate_node(node_element, "GameModel");
				
				/* Attributes */
				gameModel->append_attribute(sceneDocument.allocate_attribute("name", _proctors.at(i)->name.c_str()));
				
				/* Position */
				xml_node<>* position = sceneDocument.allocate_node(node_element, "Position");
				/* x */
				xml_node<>* x = sceneDocument.allocate_node(node_element, "x");
				x->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->getPosition().x).c_str()));
				position->append_node(x);
				/* y */
				xml_node<>* y = sceneDocument.allocate_node(node_element, "y");
				y->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->getPosition().y).c_str()));
				position->append_node(y);
				/* z */
				xml_node<>* z = sceneDocument.allocate_node(node_element, "z");
				z->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->getPosition().z).c_str()));
				position->append_node(z);
				gameModel->append_node(position);
				
				/* Rotation */
				xml_node<>* rotation = sceneDocument.allocate_node(node_element, "Rotation");
				xml_node<>* w = sceneDocument.allocate_node(node_element, "w");
				w->value(sceneDocument.allocate_string("1.0"));
				rotation->append_node(w);
				x = sceneDocument.clone_node(x);
				x->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->transform.rotation.x).c_str()));
				rotation->append_node(x);
				y = sceneDocument.clone_node(y);
				y->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->transform.rotation.y).c_str()));
				rotation->append_node(y);
				z = sceneDocument.clone_node(z);
				z->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->transform.rotation.z).c_str()));
				rotation->append_node(z);
				gameModel->append_node(rotation);
				
				/* Scale */
				xml_node<>* scale = sceneDocument.allocate_node(node_element, "Scale");
				x = sceneDocument.clone_node(x);
				x->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->getScale().x).c_str()));
				scale->append_node(x);
				/* y */
				y = sceneDocument.clone_node(y);
				y->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->getScale().y).c_str()));
				scale->append_node(y);
				/* z */
				z = sceneDocument.clone_node(z);
				z->value(sceneDocument.allocate_string(std::to_string(_proctors.at(i)->getScale().z).c_str()));
				scale->append_node(z);
				gameModel->append_node(scale);

				/* Components */
				xml_node<>* components = sceneDocument.allocate_node(node_element, "components");
				/* isColliderOn */
				xml_node<>* isColliderOn = sceneDocument.allocate_node(node_element, "isColliderOn");
				isColliderOn = sceneDocument.clone_node(isColliderOn);
				if (_proctors.at(i)->getComponentOfType(GameLogic::C_COLLIDER) != NULL)
					isColliderOn->value(sceneDocument.allocate_string("true"));
				else
					isColliderOn->value(sceneDocument.allocate_string("false"));
				components->append_node(isColliderOn);
				/* isColliderStatic */
				xml_node<>* isColliderStatic = sceneDocument.allocate_node(node_element, "isColliderStatic");
				isColliderStatic = sceneDocument.clone_node(isColliderStatic);
				if (_proctors.at(i)->getComponentOfType(GameLogic::C_COLLIDER) != NULL)
					isColliderStatic->value(sceneDocument.allocate_string("true"));
				else
					isColliderStatic->value(sceneDocument.allocate_string("false"));
				components->append_node(isColliderStatic);
				/* isInteractable */
				xml_node<>* isInteractable = sceneDocument.allocate_node(node_element, "isInteractable");
				isInteractable = sceneDocument.clone_node(isInteractable);
				if (_proctors.at(i)->getComponentOfType(GameLogic::C_INTERACTABLE) != NULL)
					isInteractable->value(sceneDocument.allocate_string("true"));
				else
					isInteractable->value(sceneDocument.allocate_string("false"));
				components->append_node(isInteractable);
				/* isTreasure */
				xml_node<>* isTreasure = sceneDocument.allocate_node(node_element, "isTreasure");
				isTreasure = sceneDocument.clone_node(isTreasure);
				if (_proctors.at(i)->getComponentOfType(GameLogic::C_TREASURE) != NULL)
					isTreasure->value(sceneDocument.allocate_string("true"));
				else
					isTreasure->value(sceneDocument.allocate_string("false"));
				components->append_node(isTreasure);
				/* isCash */
				xml_node<>* isCash = sceneDocument.allocate_node(node_element, "isCash");
				isCash = sceneDocument.clone_node(isCash);
				if (_proctors.at(i)->getComponentOfType(GameLogic::C_CASH) != NULL)
					isCash->value(sceneDocument.allocate_string("true"));
				else
					isCash->value(sceneDocument.allocate_string("false"));
				components->append_node(isCash);

				gameModel->append_node(components);

				
				

				/* Children */
				xml_node<>* childrenModels = sceneDocument.allocate_node(node_element, "ChildrenModels");

				if (_proctors.at(i)->childCount() > 0)
				{
					xml_node<>* childModel = sceneDocument.allocate_node(node_element, "ChildModel");
					childrenModels->append_node(childModel);
				}
				
				gameModel->append_node(childrenModels);
				
				/* Append GameModel to GameModels Element*/
				gameModels->append_node(gameModel);
			}
			
			/* Append GameModels to GameModelsCollection */
			gameModelsCollection->append_node(gameModels);

			/* Now append it all to the document */
			sceneDocument.append_node(gameModelsCollection);

			/* Save to file */
			std::ofstream file_stored(_path);
			file_stored << sceneDocument;
			file_stored.close();
			sceneDocument.clear();
		}
	};
}