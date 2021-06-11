#include "Animation.h"
//
//namespace AnimationSystem
//{
	Animation::Animation(const std::string& _animationPath, Loader::Model* _model, int _index)
	{
		Assimp::Importer importer;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
		const aiScene* scene = importer.ReadFile(_animationPath, aiProcess_Triangulate);
		assert(scene && scene->mRootNode);

		auto animation = scene->mAnimations[_index];
		duration = animation->mDuration;
		ticksPerSecond = animation->mTicksPerSecond;
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		readHierarchyData(rootNode, scene->mRootNode);
		setupBones(animation, *_model);
	}

	Animation::~Animation()
	{ }

	Bone* Animation::findBone(const std::string& _name)
	{
		auto iter = std::find_if(bones.begin(), bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.getBoneName() == _name;
			}
		);

		if (iter == bones.end())
			return nullptr;
		else 
			return &(*iter);
	}

	void Animation::readHierarchyData(AssimpNodeData& _dest, const aiNode* _src)
	{
		assert(_src);

		_dest.name = _src->mName.data;
		_dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(_src->mTransformation);
		_dest.childrenCount = _src->mNumChildren;

		int size = _src->mNumChildren;
		for (int i = 0; i < size; i++)
		{
			AssimpNodeData newData;
			readHierarchyData(newData, _src->mChildren[i]);
			_dest.children.push_back(newData);
		}
	}

	void Animation::setupBones(const aiAnimation* _animation, Loader::Model& _model)
	{
		int size = _animation->mNumChannels;

		auto& boneInfoMap = _model.getBoneInfoMap();
		int& boneCount = _model.getBoneCount();

		for (int i = 0; i < size; i++)
		{
			auto channel = _animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
					
			std::cout << boneName << std::endl;

			bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
		}

		std::cout << boneInfoMap.size() << std::endl;
		std::cout << boneCount << std::endl;

		this->boneInfoMap = boneInfoMap;
	}
//}