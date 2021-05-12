#include "Bone.h"

/* Reads keyframes from aiNodeAnim */
Bone::Bone(const std::string& _name, int _ID, const aiNodeAnim* _channel)
	:	name(_name), ID(_ID), localTransform(1.0f)
{
	/* Load positions */
	numPositions = _channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < numPositions; positionIndex++)
	{
		aiVector3D aiPosition = _channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = _channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
		data.timeStamp = timeStamp;
		positions.push_back(data);
	}

	/* Load rotations */
	numRotations = _channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < numRotations; rotationIndex++)
	{
		aiQuaternion aiRotation = _channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = _channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.rotation = AssimpGLMHelpers::GetGLMQuat(aiRotation);
		data.timeStamp = timeStamp;
		rotations.push_back(data);
	}

	/* Load scales */
	numScales = _channel->mNumScalingKeys;
	for (int scaleIndex = 0; scaleIndex < numScales; scaleIndex++)
	{
		aiVector3D aiScale = _channel->mScalingKeys[scaleIndex].mValue;
		float timeStamp = _channel->mScalingKeys[scaleIndex].mTime;
		KeyScale data;
		data.scale = AssimpGLMHelpers::GetGLMVec(aiScale);
		data.timeStamp = timeStamp;
		scales.push_back(data);
	}
}

/* Interpolates bones/weights positions, rotations & scales keys based on the current time of the animation
	and prepeares the local transformation matrix by comining all keys transformations
*/
void Bone::update(float _animationTime)
{
	glm::mat4 position = interpolatePosition(_animationTime);
	glm::mat4 rotation = interpolateRotation(_animationTime);
	glm::mat4 scale = interpolateScale(_animationTime);
	localTransform = position * rotation * scale;
}

/* Gets the current index on KeyPositions to interpolate to based on the current animation time */
int Bone::getPositionIndex(float _animationTime)
{
	for (int index = 0; index < numPositions - 1; index++)
	{
		if (_animationTime < positions[index + 1].timeStamp)
			return index;
	}

	assert(0);
}

/* Gets the current index on KeyRotations to interpolate to based on the current animation time */
int Bone::getRotationIndex(float _animationTime)
{
	for (int index = 0; index < numRotations - 1; index++)
	{
		if (_animationTime < rotations[index + 1].timeStamp)
			return index;
	}

	assert(0);
}

/* Gets the current index on KeyScale to interpolate to based on the current animation time */
int Bone::getScaleIndex(float _animationTime)
{
	for (int index = 0; index < numScales - 1; index++)
	{
		if (_animationTime < scales[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

/* Gets normalized value for Lerp & Slerp */
float Bone::getScaleFactor(float _lastTimeStamp, float _nextTimeStamp, float _animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = _animationTime - _lastTimeStamp;
	float framesDiff = _nextTimeStamp - _lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

/* Figures out which position keys to interpolate bone/weight and performs the interpolation
*  and returns the translation matrix
*/
glm::mat4 Bone::interpolatePosition(float _animationTime)
{
	if (1 == numPositions)
		return glm::translate(glm::mat4(1.0f), positions[0].position);

	int pZeroIndex = getPositionIndex(_animationTime);
	int pOneIndex = pZeroIndex + 1;
	float scaleFactor = getScaleFactor(positions[pZeroIndex].timeStamp, positions[pOneIndex].timeStamp, _animationTime);
	glm::vec3 finalPosition = glm::mix(positions[pZeroIndex].position, positions[pOneIndex].position, scaleFactor);

	return glm::translate(glm::mat4(1.0f), finalPosition);
}

/* Figures out which rotations keys to interpolate bone/weight and performs the interpolation
*  and rotation matrix 
*/
glm::mat4 Bone::interpolateRotation(float _animationTime)
{
	if (1 == numRotations)
	{
		auto rotation = glm::normalize(rotations[0].rotation);
		return glm::toMat4(rotation);
	}

	int rZeroIndex = getRotationIndex(_animationTime);
	int rOneIndex = rZeroIndex + 1;
	float scaleFactor = getScaleFactor(rotations[rZeroIndex].timeStamp, rotations[rOneIndex].timeStamp, _animationTime);
	glm::quat finalRotation = glm::slerp(rotations[rZeroIndex].rotation, rotations[rOneIndex].rotation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);

	return glm::toMat4(finalRotation);
}

/* Figures out which scale keys to interpolate bone/weight and performs the interpolation
*  and return the scale matrix
*/
glm::mat4 Bone::interpolateScale(float _animationTime)
{
	if (1 == numScales)
		return glm::scale(glm::mat4(1.0f), scales[0].scale);

	int sZeroIndex = getScaleIndex(_animationTime);
	int sOneIndex = sZeroIndex + 1;
	float scaleFactor = getScaleFactor(scales[sZeroIndex].timeStamp, scales[sOneIndex].timeStamp, _animationTime);
	glm::vec3 finalScale = glm::mix(scales[sZeroIndex].scale, scales[sOneIndex].scale, scaleFactor);

	return glm::scale(glm::mat4(1.0f), finalScale);
}
