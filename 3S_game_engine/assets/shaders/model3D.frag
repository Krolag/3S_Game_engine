#version 430
layout(binding = 0) uniform sampler2D diffuse0;
layout(binding = 1) uniform sampler2D specular0;

struct Material
{
    vec3 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

struct DirLight
{
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform DirLight dirLight;
uniform Material material;
uniform int noTex;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

float shadowCalc(float _dotLightNormal)
{
    /* Transform from [-1, 1] range to [0, 1] range */
    vec3 pos = FragPosLightSpace.xyz * 0.5f + 0.5f;
    if (pos.z > 1.0f)
        pos.z = 1.0f;
    float depth = texture(shadowMap, pos.xy).r;
    float bias = max(0.05f * (1.0f - _dotLightNormal), 0.005f);
    return (depth + bias) < pos.z ? 0.0f : 1.0f;
}

void main()
{
    /* Dir-light calculations */
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 diffMap, specMap;

    /* Check if model has textures */
    if (noTex == 1)
    {
        diffMap = material.diffuse;
        specMap = material.specular;
    }
    else
    {
        diffMap = texture(diffuse0, TexCoord);
        specMap = texture(specular0, TexCoord);
    }

    /* Ambient */
    vec4 ambient = dirLight.ambient * material.diffuse;
    /* Diffuse */
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec4 diffuse = dirLight.diffuse * (diff * material.diffuse);
    /* Specular */
    vec4 specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    /* If diff <= 0, the object is "behind" light source */
    if (diff > 0)
    {
        float dotProd = 0.0f;

        /* Calculate using Blinn-Phong model */
        vec3 halfwayDir = normalize(lightDir + viewDir);
        dotProd = dot(normal, halfwayDir);

        float spec = pow(max(dotProd, 0.0f), material.shininess * 128);
        specular = dirLight.specular * (spec * material.specular);
    }

    FragColor = vec4(ambient + diffuse + specular);
}