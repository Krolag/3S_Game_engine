#include "Hierarchy.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

namespace GameLogic
{
	Hierarchy::Hierarchy(bool _state)
		: active(_state), collidersLoaded(false)
	{ }

	Hierarchy::Hierarchy(Application::Scene* _scene, Camera* _camera, bool _state)
		: scene(_scene), active(_state), collidersLoaded(false), camera(_camera)
	{ }

	void Hierarchy::addObject(Proctor* _proctor)
	{
		/* Check if given ohject is already assigned to hierarchy */
		for (unsigned int i = 0; i < proctors.size(); i++)
		{
			if (proctors[i] == _proctor)
			{
				return;
			}
		}
		_proctor->setParent(this);

		/* If none found, assign it to objects vector */
		proctors.push_back(_proctor);

		/* Check if proctor has a Interactable component */
		if (_proctor->getComponentOfType(C_INTERACTABLE) != NULL)
		{
			interactable.push_back(_proctor);
		}
		// TODO: @Ignacy - tak samo tutaj jak w Hierarchy.h line 52
		if (_proctor->getComponentOfType(C_TREASURE) != NULL)
		{
			treasure.push_back(_proctor);
		}		
		if (_proctor->getComponentOfType(C_CASH) != NULL)
		{
			cash.push_back(_proctor);
		}
	}

	void Hierarchy::removeObject(Proctor* _proctor)
	{
		/* Check if any given proctor is in use */
		for (unsigned int i = 0; i < proctors.size(); i++)
		{
			/* If given proctor is in use, remove it */
			if (proctors[i] == _proctor)
			{
				proctors.erase(proctors.begin() + i);
			}
		}
	}

	Proctor* Hierarchy::getObjectsInRadiusOf(Proctor* _proctor, float _radius)
	{
		std::vector<Proctor*> tmp = _proctor->getParentHierarchy()->getCash();
		for (int i = 0; i < tmp.size(); i++)
		{
			float xDistance = tmp.at(i)->getPosition()[0] - _proctor->getPosition()[0];
			float zDistance = tmp.at(i)->getPosition()[2] - _proctor->getPosition()[2];
			float distance = sqrt(xDistance * xDistance + zDistance * zDistance);
			
			if (distance <= _radius)
			{
				return tmp.at(i);
			}
		}
	}

	Proctor* Hierarchy::getObject(std::string _name)
	{
		/* Check if proctor with given name is in use */
		for (unsigned int i = 0; i < proctors.size(); i++)
		{
			/* If proctor with given name is in use, return it */
			if (proctors[i]->name == _name)
			{
				return proctors[i];
			}
		}
	}

	Proctor* Hierarchy::getObject(unsigned int _uuid)
	{
		/* Check if proctor with given uuid is in use */
		for (unsigned int i = 0; i < proctors.size(); i++)
		{
			/* If proctor with given uuid is in use, return it */
			if (proctors[i]->uuid == _uuid)
			{
				return proctors[i];
			}
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

	void Hierarchy::drawHierarchyWindow()
	{
		/* Draw hierarchy as buttons */
		ImGui::Begin("Hierarchy");
		for (auto& a : proctors)
		{
			if (ImGui::Button(a->name.c_str(), { 150.0f, 25.0f }))
			{
				activeProctorName = a->name;
			}
		}
		ImGui::End();

		/* Draw proctor window */
		ImGui::Begin("Proctor");
		if (activeProctorName != "none")
		{
			getObject(activeProctorName)->drawDebugWindow();
		}
		ImGui::End();
	}

	void Hierarchy::cleanup()
	{
		for (auto& a : proctors)
		{
			a->cleanup();
		}
	}

	void Hierarchy::update(bool _onlyRender, bool _drawDebug)
	{
		/* Update all proctors in objects vector */
		for (auto& a : proctors)
		{
			a->update(_onlyRender);
			
			/* Check if delta time should be set */
			if (!_onlyRender)
			{
				a->setDeltaTime(scene->deltaTime);
			}
		}

		/* Check if hierarchy should be only rendering meshes */
		if (!_onlyRender)
		{
			/* At first, update scene time */
			scene->update();

			/* Check for collisions */
			for (int i = 0; i < proctors.size(); ++i)
			{

			}

			/* If hierarchy is active and _drawDebug is true, draw debug window */
			if (active && _drawDebug)
			{
				drawHierarchyWindow();
			}
		}
	}
}