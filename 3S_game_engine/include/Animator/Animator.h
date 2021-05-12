#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "Animation.h"

class Animator
{
public:
	Animator(Animation* _animation);

	void updateAniamtion(float _dt);
	void playAnimation(Animation* _pAnimation);
	void calculateBoneTransform(const AssimpNodeData* _node, glm::mat4 _parentTransform);
	std::vector<glm::mat4> getFinalBoneMatrices();

private:
	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime;
	float deltaTime;
};

#endif // !ANIMATOR_H
