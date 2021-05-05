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

		/* If none found, assing it to objects vector */
		proctors.push_back(_proctor);

		/* Check if proctor has a Interactable component */
		if (_proctor->getComponentOfType(C_INTERACTABLE)->type == C_INTERACTABLE)
		{
			interactable.push_back(_proctor);
		}
		// TODO: @Ignacy - tak samo tutaj jak w Hierarchy.h line 51
		if (_proctor->getComponentOfType(C_TREASURE)->type == C_TREASURE)
		{
			treasure.push_back(_proctor);
		}		
		if (_proctor->getComponentOfType(C_CASH)->type == C_CASH)
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


	// COLLIDER METHODS
	
	bool Hierarchy::checkAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b)
	{
		if (glm::abs(_a->transform.position.x - _b->transform.position.x) >
			(((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x +
				((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x))
		{
			return false;
		}
		if (glm::abs(_a->transform.position.y - _b->transform.position.y) >
			(((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y +
				((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y))
		{
			return false;
		}
		if (glm::abs(_a->transform.position.z - _b->transform.position.z) >
			(((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z +
				((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z))
		{
			return false;
		}
		return true;
	}

	void Hierarchy::separateAABBCollision(GameLogic::Proctor* _a, GameLogic::Proctor* _b)
	{
		/* Calculate separation direction */
		glm::vec3 separationDirection;
		if (_a->transform.position.x >= _b->transform.position.x) separationDirection.x = 1.0f;
		else separationDirection.x = -1.0f;
		if (_a->transform.position.y >= _b->transform.position.y) separationDirection.y = 1.0f;
		else separationDirection.y = -1.0f;
		if (_a->transform.position.z >= _b->transform.position.z) separationDirection.z = 1.0f;
		else separationDirection.z = -1.0f;

		/* Calculate distance between objects positions */
		glm::vec3 distanceAB;
		distanceAB.x = glm::abs(_a->transform.position.x - _b->transform.position.x);
		distanceAB.y = glm::abs(_a->transform.position.y - _b->transform.position.y);
		distanceAB.z = glm::abs(_a->transform.position.z - _b->transform.position.z);

		BoxCollider* acol = ((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER));
		BoxCollider* bcol = ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER));
		
		/* Calculate the sum of radiuses of the both objects */
		glm::vec3 radiusABSum;
		radiusABSum.x = acol->getRadius().x + bcol->getRadius().x;
		radiusABSum.y = acol->getRadius().y + bcol->getRadius().y;
		radiusABSum.z = acol->getRadius().z + bcol->getRadius().z;

		/* Calculate separation vector */
		glm::vec3 separationVector;
		separationVector.x = (radiusABSum.x - distanceAB.x + 0.01f);
		separationVector.y = (radiusABSum.y - distanceAB.y + 0.01f);
		separationVector.z = (radiusABSum.z - distanceAB.z + 0.01f);

		/* Add separation vector to the objects */
		if (glm::min(separationVector.x, separationVector.z) == separationVector.x) _a->transform.position.x += separationVector.x * separationDirection.x;
		else _a->transform.position.z += separationVector.z * separationDirection.z;
		if (checkAABBCollision(_a, _b)) _a->transform.position.y += separationVector.y * separationDirection.y;
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
			if (ImGui::Button(a->name.c_str(), { 100.0f, 25.0f }))
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
			a->update();
			
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
				if (!((BoxCollider*)proctors[i]->getComponentOfType(C_COLLIDER))->isStatic)
				{
					for (int j = 0; j < proctors.size(); ++j)
					{
						if (checkAABBCollision(proctors[i], proctors[j]))
						{
							if (proctors[i] != proctors[j]) separateAABBCollision(proctors[i], proctors[j]);
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