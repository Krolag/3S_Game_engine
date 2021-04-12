#pragma once
#ifndef SCENEGRAPH_H
/* Framework includes */
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
/* Other includes */
#include <memory>
#include <vector>

class SceneGraph
{
public:
	SceneGraph(
		const glm::vec3& _position = glm::vec3(0.0f),
		const glm::vec3& _rotation = glm::vec3(0.0f),
		const glm::vec3& _scale = glm::vec3(1.0f)
		);
	~SceneGraph();

	void setPosition(float _x, float _y, float _z);
	void setPosition(const glm::vec3& _value);
	void setRotation(float _x, float _y, float _z);
	void setRotation(const glm::mat4& _value);
	void setScale(float _x, float _y, float _z);
	void setScale(float _value);

	glm::vec3 getPosition() const { return position; }
	glm::quat getRotation() const { return rotation; }
	glm::vec3 getScale() const { return scale; }
	glm::mat4 getTransform() const { return transform; }

	void addChild(const std::shared_ptr<SceneGraph>& _child);
	void update(const glm::mat4& _parentTransform, bool _isDirty);

private:
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	glm::mat4 transform;
	bool isDirty;

	std::vector<std::shared_ptr<SceneGraph>> children;
	glm::mat4 getUpdatedTransform() const;
};

typedef std::shared_ptr<SceneGraph> SceneGraphSP;

#endif // !SCENEGRAPH_H
