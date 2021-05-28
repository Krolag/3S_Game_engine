#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

const int MAX_BONES = 64;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform vec4 plane;

void main()
{
    vec4 worldPosition = vec4(aPos, 1.0f);//model * vec4(aPos, 1.0f);
    vec4 worldNormal = vec4(aNormal, 0.0f);//mat3(transpose(inverse(model))) * aNormal;
    vec4 skinPos = {0.0f, 0.0f, 0.0f, 0.0f};
    vec4 skinNorm = {0.0f, 0.0f, 0.0f, 0.0f};

    /* Calculate total position */
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (aBoneIDs[i] == -1)
            continue;
        if (aBoneIDs[i] >= MAX_BONES)
        {
            worldPosition = model * vec4(aPos, 1.0f);
            break;
        }

        const mat4 bone = finalBonesMatrices[aBoneIDs[i]];
        const float weight = aWeights[i];

        skinPos += (bone * worldPosition) * weight;
        skinNorm += (bone * worldNormal) * weight;

//        vec4 localPosition = finalBonesMatrices[aBoneIDs[i]] * vec4(aPos, 1.0f);
//        worldPosition += localPosition * aWeights[i];
//        vec3 localNormal = mat3(finalBonesMatrices[aBoneIDs[i]]) * aNormal;
//        worldNormal += localNormal * aWeights[i];
    }

    skinPos.w = 1.0f;

    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = skinNorm.xyz;
    TexCoord = aTexCoords;

    gl_ClipDistance[0] = dot(worldPosition, plane);
    gl_Position = projection * view * model * worldPosition;
}