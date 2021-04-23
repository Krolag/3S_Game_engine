#include "Proctor.h"
#include <iostream>
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "Application/Randomizer.h"

namespace GameLogic
{
	Proctor::Proctor()
	{
		/* Assign values to transform */
		transform.position = glm::vec3(0.0f);
		transform.rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
		transform.scale = glm::vec3(1.0f);
	}

	Proctor::Proctor(const char* _name, unsigned int _uuid, Proctor* _parent, bool _active, bool _isStatic)
		: name(_name), uuid(_uuid), parent(_parent), active(_active), isStatic(_isStatic)
	{
		/* Assign uuid */
		if (uuid == 0)
		{
			uuid = uuid = Application::Randomizer().randomInt();
		}

		/* Assign values to transform */
		transform.position = glm::vec3(0.0f);
		transform.rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
		transform.scale = glm::vec3(1.0f);
	}

	Proctor::~Proctor() { }

	void Proctor::update()
	{
		/* First update components for current hierarchy level */
		for (auto& a : components)
		{
			a->update();
		}

		/* Next update children */
		for (auto& a : children)
		{
			a->update();
		}
	}

	void Proctor::cleanup()
	{
		/* Clear data from memory */
		for (auto& a : components)
		{
			a->cleanup();
		}

		for (auto& a : children)
		{
			a->cleanup();
		}
	}

	void Proctor::addChild(Proctor* _proctor)
	{
		/* Check if given proctor is already in use */
		for (unsigned int i = 0; i < children.size(); i++)
		{
			if (children[i] == _proctor)
			{
				return;
			}
		}

		/* If proctor cannot be found in children vector, add it */
		children.push_back(_proctor);
	}

	void Proctor::removeChild(Proctor* _proctor)
	{
		/* Find correct proctor and delete it */
		for (unsigned int i = 0; i < children.size(); i++)
		{
			if (children[i] == _proctor)
			{
				children.erase(children.begin() + i);
				return;
			}
		}
	}

	void Proctor::removeChildren()
	{
		/* Drop all children */
		children.clear();
	}

	Proctor* Proctor::getParent() const
	{
		return parent;
	}

	void Proctor::setParent(Proctor* _proctor)
	{
		parent = _proctor;
	}

	unsigned int Proctor::childCount()
	{
		return children.size();
	}

	void Proctor::addComponent(Component* _component)
	{
		/* Check if given component is already in use */
		for (unsigned int i = 0; i < components.size(); i++)
		{
			if (components[i] == _component)
			{
				return;
			}
		}

		/* If component cannot be found in components vector, add it */
		components.push_back(_component);
	}

	void Proctor::drawDebugWindow()
	{
		ImGui::Text("|--------| NAME |-------------|");
		ImGui::Text(name.c_str());
		ImGui::Text("|--------| TRANSFORM |--------|");
		float variables[3];
		/* Position */
		variables[0] = transform.position.x; variables[1] = transform.position.y; variables[2] = transform.position.z;
		ImGui::Text("Position:"); ImGui::SameLine(); ImGui::DragFloat3("P", variables, 0.01f);
		setPosition(glm::vec3(variables[0], variables[1], variables[2]));
		/* Rotation */
		variables[0] = transform.rotation.x; variables[1] = transform.rotation.y; variables[2] = transform.rotation.z;
		ImGui::Text("Rotation:"); ImGui::SameLine(); ImGui::DragFloat3("R", variables, 0.01f);
		setRotation(glm::quat(1.0f, variables[0], variables[1], variables[2]));
		/* Scale */
		variables[0] = transform.scale.x; variables[1] = transform.scale.y; variables[2] = transform.scale.z;
		ImGui::Text("Scale:   "); ImGui::SameLine(); ImGui::DragFloat3("S", variables, 0.01f);
		setScale(glm::vec3(variables[0], variables[1], variables[2]));
		/* Movement */
		for (auto a : components)
		{
			if (a->type == GameLogic::C_MESH)
			{
				ImGui::Text("|--------| MESHRENDERER |-----|");
				ImGui::Text("Model from path:");
				ImGui::Text(((MeshRenderer*) a)->model->path.c_str());
			}

			if (a->type == GameLogic::C_MOVEMENT)
			{
				ImGui::Text("|--------| MOVEMENT |---------|");
				if (((PlayerInput*)a)->getScheme())
				{
					ImGui::Text("Up:    W"); ImGui::SameLine();
					ImGui::Text("Down:  S"); ImGui::SameLine();
					ImGui::Text("Left:  A"); ImGui::SameLine();
					ImGui::Text("Right: D");
					ImGui::Text("Use:   V"); ImGui::SameLine();
					ImGui::Text("Row:   B");
					}
				else
				{
					ImGui::Text("Up:    I");
					ImGui::Text("Down:  K");
					ImGui::Text("Left:  J");
					ImGui::Text("Right: L");
					ImGui::Text("Use:   .");
					ImGui::Text("Row:   /");
				}
				ImGui::Text("Speed:  "); ImGui::SameLine();
				ImGui::DragFloat("s", &((PlayerInput*)a)->movementSpeed);
			}
		}
	}

	void Proctor::setTransform(glm::vec3 _position, glm::quat _rotation, glm::vec3 _scale)
	{
		transform.position = _position;
		transform.rotation = _rotation;
		transform.scale = _scale;
	}

	void Proctor::setTransform(Transform _transform)
	{
		transform = _transform;
	}

	void Proctor::setPosition(glm::vec3 _position)
	{
		transform.position = _position;
	}

	void Proctor::setRotation(glm::quat _rotation)
	{
		transform.rotation = _rotation;
	}

	void Proctor::setScale(glm::vec3 _scale)
	{
		transform.scale = _scale;
	}

	Transform Proctor::getTransform()
	{
		return transform;
	}

	glm::vec3 Proctor::getPosition()
	{
		return transform.position;
	}

	glm::quat Proctor::getRotation()
	{
		return transform.rotation;
	}

	glm::vec3 Proctor::getScale()
	{
		return transform.scale;
	}
}