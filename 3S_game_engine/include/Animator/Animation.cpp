#include "Animation.h"

Animation::Animation(const std::string& _animationPath, Loader::Model* _model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;
	readHierarchyData(rootNode, scene->mRootNode);
	readMissingBones(animation, *_model);
}

Bone* Animation::findBone(const std::string& _name)
{
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&](Bone& Bone) // TODO: @Dawid const?
		{
			return Bone.getBoneName() == _name;
		}
	);

	if (iter == bones.end())	return nullptr;
	else return &(*iter);
}

void Animation::readMissingBones(const aiAnimation* _animation, Loader::Model& _model)
{
	int size = _animation->mNumChannels;

	auto& localBoneInfoMap = _model.getBoneInfoMap();
	int& boneCount = _model.getBoneCounter();

	/* Reading channels (bones engaged in an animation and their keyframes) */
	for (int i = 0; i < size; i++)
	{
		auto channel = _animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (localBoneInfoMap.find(boneName) == localBoneInfoMap.end())
		{
			localBoneInfoMap[boneName].id = boneCount;
			boneCount++;
		}

		bones.push_back(Bone(channel->mNodeName.data, localBoneInfoMap[channel->mNodeName.data].id, channel));
	}

	//boneInfoMap = localBoneInfoMap;
}

void Animation::readHierarchyData(AssimpNodeData& _dest, const aiNode* _src)
{
	assert(_src);

	_dest.name = _src->mName.data;
	_dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(_src->mTransformation);
	_dest.childrenCount = _src->mNumChildren;

	for (int i = 0; i < _src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		readHierarchyData(newData, _src->mChildren[i]);
		_dest.children.push_back(newData);
	}
}