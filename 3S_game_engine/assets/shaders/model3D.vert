#version 430 core
const int MAX_JOINTS = 100;
const int MAX_BONE_INFLUENCE = 4;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in ivec3 inJointIndices;
layout (location = 4) in vec3 inWeights;

out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;
uniform mat4 jointTransforms[100];
uniform bool noAnim;
uniform mat4 lightSpaceMatrix;

void main()
{
	vec4 worldPosition = model * vec4(inPos, 1.0f);
	gl_ClipDistance[0] = dot(worldPosition, plane);

	/* Check if model has animation */
	if (noAnim)
	{
		FragPos = vec3(model * vec4(inPos, 1.0f));
		Normal = mat3(transpose(inverse(model))) * inNormal;
		TexCoord = inTexCoords;
		FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);

		gl_Position = projection * view * vec4(FragPos, 1.0f);
	}
	else
	{
		vec4 totalLocalPosition = vec4(0.0f);
		vec3 totalNormal = vec3(0.0f);

		/* Calculate total position */
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			mat4 jointTransformTMP = jointTransforms[inJointIndices[i]];
			vec4 posePosition = jointTransformTMP * vec4(inPos, 1.0f);
			totalLocalPosition += posePosition * inWeights[i];

//			vec4 localPosition = jointTransforms[inJointIndices[i]] * vec4(inPos, 1.0f);
//			totalLocalPosition += localPosition * inWeights[i];
//
//			vec4 worldNormal = jointTransforms[inJointIndices[i]] * vec4(inNormal, 1.0f);
			totalNormal = mat3(jointTransforms[inJointIndices[i]]) * inNormal;
		}

		mat4 viewMovel = view * model;
		gl_Position = projection * viewMovel * totalLocalPosition;
		TexCoord = inTexCoords;
		FragPos = vec3(model * vec4(inPos, 1.0f));

		Normal = mat3(transpose(inverse(model))) * totalNormal;
		FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
		
	}
}