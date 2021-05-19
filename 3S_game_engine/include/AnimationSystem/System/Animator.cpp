#include "Animator.h"

//namespace AnimationSystem
//{
	Animator::Animator(Animation* _animation)
	{
		this->currentAnimation = _animation;
		this->currentTime = 0.0f;
		finalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
		{
			finalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		playAnimation(_animation);
	}

	void Animator::updateAniamtion(float _dt)
	{
		this->deltaTime = _dt;

		if (this->currentAnimation)
		{
			currentTime += currentAnimation->getTicksPerSecond() * _dt;
			currentTime = fmod(currentTime, currentAnimation->getDuration());
			calculateBoneTransform(&currentAnimation->getRootNode(), glm::mat4(1.0f));
		}
	}

	void Animator::playAnimation(Animation* _pAnimation)
	{
		currentAnimation = _pAnimation;
		currentTime = 0.0f;
	}

	void Animator::calculateBoneTransform(const AssimpNodeData* _node, glm::mat4 _parentTransform)
	{
		std::string nodeName = _node->name;
		glm::mat4 nodeTransform = _node->transformation;

		Bone* bone = currentAnimation->findBone(nodeName);

		if (bone)
		{
			bone->update(currentTime);
			nodeTransform = bone->getLocalTransform();
		}

		glm::mat4 globalTransform = _parentTransform * nodeTransform;

		auto boneInfoMap = currentAnimation->getBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			finalBoneMatrices[index] = globalTransform * offset;
		}

		for (int i = 0; i < _node->childrenCount; i++)
			calculateBoneTransform(&_node->children[i], globalTransform);
	}

	std::vector<glm::mat4> Animator::getFinalBoneMatrices()
	{
		return finalBoneMatrices;
	}
//}