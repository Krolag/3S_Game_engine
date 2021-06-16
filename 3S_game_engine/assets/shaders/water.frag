#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec4 clipSpace;
in vec3 toCamera;
in vec3 fromLight;

in vec3 pass_normal;
in vec4 pass_clipSpaceReal;
in vec4 pass_clipSpaceGrid;
in vec3 pass_toCameraVector;
in vec3 normalG;

uniform sampler2D reflectionTexture;
uniform vec3 lightPos;
uniform vec3 waterColor;

vec3 lightColor = vec3(1.0,1.0,1.0);

vec2 clipSpaceToTexCoords(vec4 clipSpace){
	vec2 ndc = (clipSpace.xy / clipSpace.w);
	vec2 texCoords = ndc / 2.0 + 0.5;
	return clamp(texCoords, 0.002, 0.998);
}

void main()
{ 
	vec2 texCoordsGrid = clipSpaceToTexCoords(pass_clipSpaceGrid);
	
	vec2 reflectionTextCoords = vec2(texCoordsGrid.x, 1.0 - texCoordsGrid.y);	
	vec3 reflectColor = texture(reflectionTexture,reflectionTextCoords).rgb;
	
	//ambient light
	float ambientStrength = 0.9;
    vec3 ambient = ambientStrength * lightColor;
	
	//specular light
	vec3 viewDir = pass_toCameraVector;
	vec3 reflectDir = reflect(normalize(lightPos), normalG);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
    vec3 specular = 0.5 * spec * lightColor; 

	vec3 finnalColor = mix(waterColor,reflectColor,0.3);
	FragColor = vec4(finnalColor,1.0) * vec4((ambient + specular),1.0);
}