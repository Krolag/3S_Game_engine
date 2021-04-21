#include "Hierarchy.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

Hierarchy::Hierarchy(bool _state)
	: active(_state)
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
	/* Update all proctors in objects vector */
	for (auto& a : objects)
	{
		a->update();
	}

	/* If hierarchy is active, draw debug window */
	if (active)
	{
		drawHierarchyWindow();
	}
}
