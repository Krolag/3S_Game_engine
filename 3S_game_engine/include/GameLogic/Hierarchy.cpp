#include "Hierarchy.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

namespace GameLogic
{
	Hierarchy::Hierarchy(bool _state)
		: active(_state), collidersLoaded(false)
	{ }

	Hierarchy::Hierarchy(Application::Scene* _scene, bool _state)
		: scene(_scene), active(_state), collidersLoaded(false)
	{ }

	void Hierarchy::addObject(Proctor* _proctor)
	{
		/* Check if given ohject is already assigned to hierarchy */
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			if (objects[i] == _proctor)
			{
				return;
			}
		}

		/* If none found, assing it to objects vector */
		objects.push_back(_proctor);
	}

	void Hierarchy::removeObject(Proctor* _proctor)
	{
		/* Check if any given proctor is in use */
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			/* If given proctor is in use, remove it */
			if (objects[i] == _proctor)
			{
				objects.erase(objects.begin() + i);
			}
		}
	}

	Proctor* Hierarchy::getObject(std::string _name)
	{
		/* Check if proctor with given name is in use */
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			/* If proctor with given name is in use, return it */
			if (objects[i]->name == _name)
			{
				return objects[i];
			}
		}
	}

	Proctor* Hierarchy::getObject(unsigned int _uuid)
	{
		/* Check if proctor with given uuid is in use */
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			/* If proctor with given uuid is in use, return it */
			if (objects[i]->uuid == _uuid)
			{
				return objects[i];
			}
		}
	}

	std::vector<Proctor*> Hierarchy::getObjects()
	{
		return objects;
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

		// TODO: get _a as boxcollider erlier for memory optimalization

		/* Calculate the sum of radians of the both objects */
		glm::vec3 radiusABSum;
		radiusABSum.x = ((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x + ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().x;
		radiusABSum.y = ((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y + ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().y;
		radiusABSum.z = ((GameLogic::BoxCollider*)_a->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z + ((GameLogic::BoxCollider*)_b->getComponentOfType(GameLogic::C_COLLIDER))->getRadius().z;

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
		for (auto& a : objects)
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
		for (auto& a : objects)
		{
			a->cleanup();
		}
	}

	void Hierarchy::update()
	{
		/* At first, update scene time */
		scene->update();

		/* Update all proctors in objects vector */
		for (auto& a : objects)
		{
			a->setDeltaTime(scene->deltaTime);
			a->update();
		}

		for (int i = 0; i < objects.size(); ++i)
		{
			if(!((BoxCollider*)objects[i]->getComponentOfType(C_COLLIDER))->isStatic)
			{
				for (int j = 0; j < objects.size(); ++j)
				{
					if (checkAABBCollision(objects[i], objects[j]))
					{
						if (objects[i] != objects[j]) separateAABBCollision(objects[i], objects[j]);
					}
				}
			}
		}

		/* If hierarchy is active, draw debug window */
		if (active)
		{
			drawHierarchyWindow();
		}
	}
}