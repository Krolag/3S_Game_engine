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
    vec3 color = material.diffuse.xyz;//texture(diffuse0, TexCoord).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(0.3);

    /* Ambient */
    vec3 ambient = dirLight.ambient.xyz * material.diffuse.xyz * color;
    /* Diffuse */
    float diff = max(dot(normal, normalize(-dirLight.direction)), 0.0f);
    vec3 diffuse = dirLight.diffuse.xyz * (diff * material.diffuse.xyz) * lightColor;
    /* Specular */
    vec3 reflectDir = reflect(-normalize(-dirLight.direction), normal);
    float spec = pow(max(dot(normalize(viewPos - FragPos), reflectDir), 0.0f), material.shininess * 128.0f);
    vec3 specular = dirLight.specular.xyz * (spec * material.specular.xyz) * lightColor;
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 halfwayDir = normalize(lightDir + viewDir);
//    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0);
//    vec3 specular = spec * lightColor;

    /* Calculate shadows */
    float shadow = shadowCalc(dot(dirLight.direction, normal));
    vec3 lighting = (shadow * (diffuse + specular) + ambient) * color;

    FragColor = vec4(lighting, 1.0f);
}