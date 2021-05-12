#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "Bone.h"
#include "Loader/Model.h"

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;
	Animation(const std::string& _animationPath, Loader::Model* _model);
	~Animation() {}

	Bone* findBone(const std::string& _name);
	inline float getTicksPerSecond() { return ticksPerSecond; }
	inline float getDuration() { return duration; }
	inline const AssimpNodeData& getRootNode() { return rootNode; }
	inline const std::map<std::string, BoneInfo>& getBoneIDMap() { return boneInfoMap; }

private:
	float duration;
	int ticksPerSecond;
	std::vector<Bone> bones;
	AssimpNodeData rootNode;
	std::map<std::string, BoneInfo> boneInfoMap;

	void readMissingBones(const aiAnimation* _animation, Loader::Model& _model);
	void readHierarchyData(AssimpNodeData& _dest, const aiNode* _src);
};

#endif // !ANIMATION_H
