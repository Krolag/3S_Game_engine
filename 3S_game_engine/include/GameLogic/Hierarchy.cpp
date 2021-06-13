#include "Hierarchy.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

namespace GameLogic
{
	Hierarchy::Hierarchy(bool _state)
		: active(_state), collidersLoaded(false)
	{ }

	Hierarchy::Hierarchy(Application::Window* _scene, Camera* _camera, bool _state)
		: scene(_scene), active(_state), collidersLoaded(false), camera(_camera)
	{ }

	void Hierarchy::addObject(Proctor* _proctor)
	{
		/* Check if given ohject is already assigned to hierarchy */
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			if (proctors[i] == _proctor)
				return;
		}
		_proctor->setParent(this);

		/* If none found, assign it to objects vector */
		proctors.push_back(_proctor);

		/* Check if proctor has a Interactable component */
		if (_proctor->getComponentOfType(C_INTERACTABLE) != NULL)
			interactable.push_back(_proctor);
		if (_proctor->getComponentOfType(C_TREASURE) != NULL)
			treasure.push_back(_proctor);
		if (_proctor->getComponentOfType(C_CASH) != NULL)
			cash.push_back(_proctor);
	}

	void Hierarchy::removeObject(Proctor* _proctor)
	{
		/* Check if any given proctor is in use */
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			/* If given proctor is in use, remove it */
			if (proctors[i] == _proctor)
				proctors.erase(proctors.begin() + i);
		}
	}

	Proctor* Hierarchy::getObjectsInRadiusOf(Proctor* _proctor, float _radius)
	{
		std::vector<Proctor*> tmp = _proctor->getParentHierarchy()->getCash();
		unsigned int size_i = tmp.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			float xDistance = tmp.at(i)->getPosition()[0] - _proctor->getPosition()[0];
			float zDistance = tmp.at(i)->getPosition()[2] - _proctor->getPosition()[2];
			float distance = sqrt(xDistance * xDistance + zDistance * zDistance);
			
			if (distance <= _radius)
				return tmp.at(i);
		}
	}

	Proctor* Hierarchy::getObject(std::string _name)
	{
		/* Check if proctor with given name is in use */
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			/* If proctor with given name is in use, return it */
			if (proctors[i]->name == _name)
				return proctors[i];

			/* Check if proctor has a child with given name*/
			unsigned int size_j = proctors.at(i)->getChildren().size();
			for (unsigned int j = 0; j < size_j; j++)
			{
				if (proctors.at(i)->getChild(j)->name == _name)
					return proctors.at(i)->getChild(j);
			}
		}

		return NULL;
	}

	Proctor* Hierarchy::getObject(unsigned int _uuid)
	{
		/* Check if proctor with given uuid is in use */
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			/* If proctor with given uuid is in use, return it */
			if (proctors[i]->uuid == _uuid)
				return proctors[i];
		}
	}

	std::vector<Proctor*> Hierarchy::getProctors()
	{
		return proctors;
	}

	std::vector<Proctor*> Hierarchy::getInteractable()
	{
		return interactable;
	}	
	
	std::vector<Proctor*> Hierarchy::getTreasure()
	{
		return treasure;
	}	
	
	std::vector<Proctor*> Hierarchy::getCash()
	{
		return cash;
	}

	float Hierarchy::getDeltaTime()
	{
		return scene->deltaTime;
	}

	void Hierarchy::setCamera(Camera* _camera)
	{
		camera = _camera;
	}

	Camera* Hierarchy::getCamera()
	{
		return camera;
	}

	void Hierarchy::setState(bool _state)
	{
		active = _state;
	}

	bool Hierarchy::getState()
	{
		return active;
	}

	void Hierarchy::renderWithShader(Shader* _shader)
	{
		/* Get mesh component from proctor and check its existance */
		MeshRenderer* mesh = (MeshRenderer*)proctors[0]->getComponentOfType(C_MESH);
		Shader* currentlyUsedShader = mesh->shader;

		/* Check if shader is the same as the old one */
		if (currentlyUsedShader == _shader)
		{
			/* Call update only for render purpose with old shader */
			update(true, false);
			return;
		}

		/* Iterate through each proctor */
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			MeshRenderer* meshTMP = (MeshRenderer*)proctors[i]->getComponentOfType(C_MESH);

			if (meshTMP != NULL)
			{
				/* Set new shader */
				meshTMP->setShader(_shader);
			}
		}

		/* Call update only for render purpose with new shader */
		update(true, false);

		/* Iterate through each proctor and set old shader */
		for (unsigned int i = 0; i < size_i; i++)
		{
			/* Get mesh component from proctor and check its existance */
			MeshRenderer* meshTMP = (MeshRenderer*)proctors[i]->getComponentOfType(C_MESH);
			if (meshTMP != NULL)
				meshTMP->setShader(currentlyUsedShader);
		}
	}

	void Hierarchy::drawHierarchyWindow()
	{
		/* Draw hierarchy as buttons */
		ImGui::Begin("Hierarchy");
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			if (ImGui::Button(proctors[i]->name.c_str(), { 150.0f, 25.0f }))
			{
				activeProctorName = proctors[i]->name;
			}
			unsigned int size_j = proctors[i]->childCount();
			for (unsigned int j = 0; i < size_j; j++)
			{
				std::string name = proctors[i]->getChild(j)->name.c_str();

				if (ImGui::Button(name.c_str(), { 150.0f, 25.0f }))
					activeProctorName = name;
			}
		}
		ImGui::End();

		/* Draw proctor window */
		ImGui::Begin("Proctor");
		if (activeProctorName != "none")
			getObject(activeProctorName)->drawDebugWindow();
		ImGui::End();
	}

	void Hierarchy::cleanup()
	{
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			proctors[i]->cleanup();
		}
	}

	void Hierarchy::update(bool _onlyRender, bool _drawDebug, int collisionIncrement)
	{
		/* Update all proctors */
		unsigned int size_i = proctors.size();
		for (unsigned int i = 0; i < size_i; i++)
		{
			proctors[i]->update(_onlyRender);
			
			/* Check if delta time should be set */
			if (!_onlyRender)
				proctors[i]->setDeltaTime(scene->deltaTime);
		}

		/* Check if hierarchy should be only rendering meshes */
		if (!_onlyRender)
		{
			/* At first, update scene time */
			//scene->update();

			/* COLLISION DETECTION */
			/* First we want to check collsiions between players, swapping order each frame */
			Proctor* hero_00 = getObject("hero_00");
			Proctor* hero_01 = getObject("hero_01");

			if(hero_00 != NULL && hero_01 != NULL)
			{
				if(hero_00->getComponentOfType(C_COLLIDER) != NULL && hero_01->getComponentOfType(C_COLLIDER) != NULL)
				{
					/* If heroes proctors exist and they have collider on them, check collisions between players */
					if (collisionIncrement % 2 == 0)
					{
						((BoxCollider*)hero_00->getComponentOfType(C_COLLIDER))->checkCollisionOBB((BoxCollider*)hero_01->getComponentOfType(C_COLLIDER));
						((BoxCollider*)hero_01->getComponentOfType(C_COLLIDER))->checkCollisionOBB((BoxCollider*)hero_00->getComponentOfType(C_COLLIDER));
					}
					else
					{
						((BoxCollider*)hero_01->getComponentOfType(C_COLLIDER))->checkCollisionOBB((BoxCollider*)hero_00->getComponentOfType(C_COLLIDER));
						((BoxCollider*)hero_00->getComponentOfType(C_COLLIDER))->checkCollisionOBB((BoxCollider*)hero_01->getComponentOfType(C_COLLIDER));
					}
				}
			}
			
			/* Collision detection loop for anything else than player-player */
			for (int i = 0; i < proctors.size(); ++i)
			{
				/* Check if proctor has collider */
				if(proctors.at(i)->getComponentOfType(C_COLLIDER) != NULL)
				{
					/* If proctor has collider on it, check if proctors collider is not static */
					if(!((BoxCollider*)proctors.at(i)->getComponentOfType(C_COLLIDER))->isStatic)
					{
						/* If proctors collider is not static, check collisions with other proctors */
						for (int j = 0; j < proctors.size(); ++j)
						{
							/* Check if j == i to not check collider on itself and check if other proctor has collider */
							if (j == i || proctors.at(j)->getComponentOfType(C_COLLIDER) == NULL)
								continue;
							
							/* Don't check collisions on players here, because we have checked it already */
							if ((proctors.at(i)->name == "hero_00" && proctors.at(j)->name == "hero_01")
								|| (proctors.at(i)->name == "hero_01" && proctors.at(j)->name == "hero_00"))
								continue;

							/* Don't check proctors with the scale of 0,0,0 */
							if (proctors.at(j)->getScale() == glm::vec3(0.0f, 0.0f, 0.0f))
								continue;
							/* Check collisions between two proctors */
							((BoxCollider*)proctors.at(i)->getComponentOfType(C_COLLIDER))->checkCollisionOBB((BoxCollider*)proctors.at(j)->getComponentOfType(C_COLLIDER));
						}
					}
				}
			}
			
			/* If hierarchy is active and _drawDebug is true, draw debug window */
			if (active && _drawDebug)
			{
				drawHierarchyWindow();
			}
		}
	}
}