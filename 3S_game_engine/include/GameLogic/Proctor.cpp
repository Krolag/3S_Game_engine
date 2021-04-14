#include "Proctor.h"

Proctor::Proctor(std::string _name, const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
{
	name = _name;
	transform.position = _position;
	transform.rotation = _rotation;
	transform.scale = _scale;
}

Proctor::~Proctor() { }

void Proctor::addChild(Proctor _child)
{
	children.push_back(_child);
}

Proctor Proctor::getChild(Proctor _child)
{
	for (auto obj : children)
	{
		if (obj.name == _child.name)
		{
			return obj;
		}
	}
}

std::vector<Proctor> Proctor::getChildren()
{
	return children;
}

//void Proctor::addComponent(Component _component)
//{
//	components.push_back(_component);
//}

void Proctor::input()
{
	//for (auto& compo : components)
	//{
	//	compo.input();
	//}

	for (auto& child : children)
	{
		child.input();
	}
}

void Proctor::update()
{
	/*for (auto& compo : components)
	{
		compo.update();
	}*/

	for (auto& child : children)
	{
		child.update();
	}
}

void Proctor::render(Shader _shader)
{
	/*for (auto& compo : components)
	{
		compo.render();
	}*/

	for (auto& child : children)
	{
		child.render(_shader);
	}
}

void Proctor::setScale(glm::vec3 _scale)
{
	transform.scale = _scale;
}
