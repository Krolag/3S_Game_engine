#include "SceneGraph.h"

SceneGraph::SceneGraph(const glm::vec3& _position, const glm::vec3& _rotation, const glm::vec3& _scale)
	: position(_position), rotation(_rotation), scale(_scale), transform(glm::mat4(1.0f)), isDirty(true)
{}

SceneGraph::~SceneGraph() { }

void SceneGraph::setPosition(float _x, float _y, float _z)
{
	position = glm::vec3(_x, _y, _z);
	isDirty = true;
}

void SceneGraph::setPosition(const glm::vec3& _value)
{
	position = _value;
	isDirty = true;
}

void SceneGraph::setRotation(float _x, float _y, float _z)
{
	rotation = 
		glm::angleAxis(glm::radians(_x), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::angleAxis(glm::radians(_y), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::angleAxis(glm::radians(_z), glm::vec3(0.0f, 0.0f, 1.0f));
	isDirty = true;
}

void SceneGraph::setRotation(const glm::mat4& _value)
{
	rotation = glm::quat_cast(_value);
	isDirty = true;
}

void SceneGraph::setScale(float _x, float _y, float _z)
{
	scale = glm::vec3(_x, _y, _z);
	isDirty = true;
}

void SceneGraph::setScale(float _value)
{
	scale = glm::vec3(_value);
	isDirty = true;
}

void SceneGraph::addChild(const std::shared_ptr<SceneGraph>& _child)
{
	children.push_back(_child);
}

void SceneGraph::update(const glm::mat4& _parentTransform, bool _isDirty)
{
	_isDirty = _isDirty | isDirty;

	if (_isDirty)
	{
		transform = getUpdatedTransform();
		transform = _parentTransform * transform;
		isDirty = false;
	}

	for (auto& child : children)
	{
		child->update(transform, _isDirty);
	}
}

glm::mat4 SceneGraph::getUpdatedTransform() const
{
	glm::mat4 P = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 R = glm::mat4_cast(rotation);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

	return P * R * S;
}
