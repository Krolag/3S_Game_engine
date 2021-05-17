#pragma once
#ifndef BONE_H
#define BONE_H

#include <vector>
#include <assimp/scene.h>
#include <assimp/AssimpGLMHelpers.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat rotation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{
public:
	Bone(const std::string& _name, int _ID, const aiNodeAnim* _channel);

	void update(float _animationTime);
	glm::mat4 getLocalTransform() { return localTransform; }
	std::string getBoneName() const { return name; }
	int getBoneID() { return ID; }
	int getPositionIndex(float _animationTime);
	int getRotationIndex(float _animationTime);
	int getScaleIndex(float _animationTime);

private:
	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;
	int numPositions;
	int numRotations;
	int numScales;
	
	glm::mat4 localTransform;
	std::string name;
	int ID;

	float getScaleFactor(float _lastTimeStamp, float _nextTimeStamp, float _animationTime);
	glm::mat4 interpolatePosition(float _animationTime);
	glm::mat4 interpolateRotation(float _animationTime);
	glm::mat4 interpolateScale(float _animationTime);
};

#endif // !BONE_H
