#version 430 core
const int MAX_JOINTS = 64;
const int MAX_WEIGHTS = 4;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in ivec3 inJointIndices;
layout (location = 4) in vec3 inWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;
uniform mat4 jointTransforms[MAX_JOINTS];
uniform bool noAnim;

void main()
{
    vec4 totalLocalPosition = vec4(0.0f);
    vec4 totalNormal = vec4(0.0f);

    /* Check if model has animation */
    if (noAnim)
    {
        totalLocalPosition = vec4(inPos, 1.0f);
        totalNormal.xyz = mat3(transpose(inverse(model))) * inNormal;
    }
    else
    {
        /* Calculate total position */
        for (int i = 0; i < MAX_WEIGHTS; i++)
        {
            if (inJointIndices[i] != -1)
            {        
                vec4 localPosition = jointTransforms[inJointIndices[i]] * vec4(inPos, 1.0f);
                totalLocalPosition += localPosition * inWeights[i];

                vec4 worldNormal = jointTransforms[inJointIndices[i]] * vec4(inNormal, 1.0f);
                totalNormal += worldNormal * inWeights[i];
            }
        }
    }

    gl_ClipDistance[0] = dot(totalLocalPosition, plane);
    gl_Position = projection * view * model * totalLocalPosition;

    FragPos = vec3(model * vec4(totalLocalPosition.xyz, 1.0f));
    Normal = totalNormal.xyz;
    TexCoord = inTexCoords;
}