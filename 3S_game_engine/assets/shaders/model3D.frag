#version 430

struct Material
{
    vec3 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

#define MAX_POINT_LIGHTS 20
struct PointLight
{
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float k0;
    float k1;
    float k2;
};

struct DirLight
{
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

#define MAX_SPOT_LIGHTS 5
struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float k0;
    float k1;
    float k2;
};

uniform Material material;
uniform int noTex;
layout(binding = 0) uniform sampler2D diffuse0;
layout(binding = 1) uniform sampler2D specular0;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;
uniform DirLight dirLight;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int noSpotLights;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 viewPosition;

vec4 calcPointLight (int _idx,   vec3 _norm,    vec3 _viewDir, vec4 _diffMap, vec4 _specMap);
vec4 calcDirLight               (vec3 _norm, vec3 _viewDir, vec4 _diffMap, vec4 _specMap);
vec4 calcSpotLight  (int _idx,   vec3 _norm,    vec3 _viewDir, vec4 _diffMap, vec4 _specMap);

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec4 diffMap;
    vec4 specMap;

    /* Check if model is using textures */
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

    vec4 result;

    /* Directional */
    result = calcDirLight(norm, viewDir, diffMap, specMap);
    /* Points */
    for (int i = 0; i < noPointLights; i++)
    {
        result += calcPointLight(i, norm, viewDir, diffMap, specMap);
    }
    /* Spots */
    for (int i = 0; i < noPointLights; i++)
    {
        result += calcSpotLight(i, norm, viewDir, diffMap, specMap);
    }

    /* Set value correctly */
    FragColor = result; //uncomment this line of code, to see model with textures
//    FragColor = vec4(TexCoord, 0.0f, 1.0f); // uncomment this line of code, to see model's texture coord
}

vec4 calcPointLight(int _idx, vec3 _norm, vec3 _viewDir, vec4 _diffMap, vec4 _specMap)
{
    vec3 lightDirection = normalize(pointLights[_idx].position - FragPos);

    /* Ambient */
    vec4 ambient = pointLights[_idx].ambient * _diffMap;
    /* Diffuse */
    float diff = max(dot(_norm, lightDirection), 0.0f);
    vec4 diffuse = pointLights[_idx].diffuse * (diff * _diffMap);
    /* Specular */
    vec3 reflectDir = reflect(-lightDirection, _norm);
    float spec = pow(max(dot(_viewDir, reflectDir), 0.0f), material.shininess * 128);
    vec4 specular = pointLights[_idx].specular * (spec * _specMap);

    /* Attenuation */ 
    float dist = length(pointLights[_idx].position - FragPos);
    float attenuation = 1.0f / (pointLights[_idx].k0 + pointLights[_idx].k1 * dist + pointLights[_idx].k2 * (dist * dist));

    /* Set value correctly */
    return vec4(ambient + diffuse + specular) * attenuation;
}

vec4 calcDirLight(vec3 _norm, vec3 _viewDir, vec4 _diffMap, vec4 _specMap)
{
    vec3 lightDirection = normalize(-dirLight.direction);

    /* Ambient */
    vec4 ambient = dirLight.ambient * _diffMap;
    /* Diffuse */
    float diff = max(dot(_norm, lightDirection), 0.0f);
    vec4 diffuse = dirLight.diffuse * (diff * _diffMap);
    /* Specular */
    vec3 reflectDir = reflect(-lightDirection, _norm);
    float spec = pow(max(dot(_viewDir, reflectDir), 0.0f), material.shininess * 128);
    vec4 specular = dirLight.specular * (spec * _specMap);

    /* Set value correctly */
    return vec4(ambient + diffuse + specular);
}

vec4 calcSpotLight(int _idx, vec3 _norm, vec3 _viewDir, vec4 _diffMap, vec4 _specMap)
{
    vec3 lightDirection = normalize(spotLights[_idx].position - FragPos);
    float theta = dot(lightDirection, normalize(-spotLights[_idx].direction));

    /* Ambient */
    vec4 ambient = spotLights[_idx].ambient * _diffMap;

    /* If in cutoff, render light */    
    if (theta > spotLights[_idx].outerCutOff) // < because using cosines and not degrees
    {
        /* Diffuse */
        float diff = max(dot(_norm, lightDirection), 0.0f);
        vec4 diffuse = spotLights[_idx].diffuse * (diff * _diffMap);

        /* Specular */
        vec3 reflectDir = reflect(-lightDirection, _norm);
        float spec = pow(max(dot(_viewDir, reflectDir), 0.0f), material.shininess * 128);
        vec4 specular = spotLights[_idx].specular * (spec * _specMap);

        /* Intensity */
        float intensity = (theta - spotLights[_idx].outerCutOff) / (spotLights[_idx].cutOff - spotLights[_idx].outerCutOff);
        intensity = clamp(intensity, 0.0f, 1.0f);
        diffuse *= intensity;
        specular *= intensity;

        /* Attenuation */ 
        float dist = length(spotLights[_idx].position - FragPos);
        float attenuation = 1.0f / (spotLights[_idx].k0 + spotLights[_idx].k1 * dist + spotLights[_idx].k2 * (dist * dist));

        /* Set value correctly */
        return vec4(ambient + diffuse + specular) * attenuation;
    }
    else
    {
        /* Render just ambient */
        return ambient;
    }
}