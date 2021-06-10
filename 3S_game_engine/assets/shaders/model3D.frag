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

    sampler2D depthBuffer;
    mat4 lightSpaceMatrix;
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
uniform vec3 viewPos;

float shadowCalc(vec3 norm, vec3 lighrDir)
{
    vec4 fragPosLightSpace = dirLight.lightSpaceMatrix * vec4(FragPos, 1.0f);

    /* Persepctive divide (transforming coordinatess NDC) */
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    /* NDC to depth range */
    projCoords = projCoords * 0.5 + 0.5; // [-1, 1] -> [0, 1]
    /* If too far from light, do not return any shadow */
    if (projCoords.z > 1.0f)
        return 0.0f;
    /* Get closest depth in depth buffer */
    float closestDepth = texture(dirLight.depthBuffer, projCoords.xy).r;
    /* Get depth of fragment */
    float currentDepth = projCoords.z;
    /* Calculate bias */
    float maxBias = 0.05f;
    float minBias = 0.005f;
    float bias = max(maxBias * (1.0f - dot(norm, lighrDir)), minBias);
    /* PCF - percentage closer filtering */
    float shadowSum = 0.0f;
    vec2 texelSize = 1.0f / textureSize(dirLight.depthBuffer, 0);
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float pcfDepth = texture(dirLight.depthBuffer, projCoords.xy + vec2(x, y) * texelSize).r;
            /* If depth is greater (further away), add 1 */
            shadowSum += currentDepth - bias > closestDepth ? 1.0f : 0.0f;
        }
    }
    /* Return average */
    return shadowSum / 9.0f;
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

    float shadow = shadowCalc(normal, lightDir);
    
    vec4 result = vec4(ambient + (1.0f - shadow) * (diffuse + specular));

    /* Gamma correction */
    if (true)
    {
        float gamma = 2.2f;
        result.rgb = pow(result.rgb, vec3(1.0f / gamma));
    }

    /* Depth testing */
    float near = 0.1f;
    float far = 1000.0f;
    float z = gl_FragCoord.z * 2.0 - 1.0f; // Transform to NDC [0, 1] => [-1, 1]
    float linearDepth = (2.0f * near * far) / (z * (far - near) - (far + near)); // Take inverse of the projection matrix (perspective)
    float factor = (near + linearDepth) / (near - far); // Convert back to [0, 1]
    result.rgb *= (1 - factor);

    FragColor = result;
}   