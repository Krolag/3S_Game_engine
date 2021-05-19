#pragma once

#include <glm/glm.hpp>

//namespace AnimationSystem
//{
	struct BoneInfo
	{
		/*
			For uniquely indentifying the bone and
			for indexing bone transformation in shaders
		*/
		int id;
		/*
			map from bone name to offset matrix.
			Offset matrix transforms bone from bone space to local space
		*/
		glm::mat4 offset;
	};
//}