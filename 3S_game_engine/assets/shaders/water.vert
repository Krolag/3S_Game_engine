#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    vec4 gpass_clipSpaceReal;
	vec4 gpass_clipSpaceGrid;
	vec3 gpass_toCameraVector;
	vec3 gpass_normal;
} vs_out;
//out vec4 pass_clipSpaceReal;
//out vec4 pass_clipSpaceGrid;
//out vec3 pass_toCameraVector;
//out vec3 pass_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform float waveSpeed;

float waveLength = 2.0f;
float waveAmplitude = 0.3;
float PI = 3.14159265;

float generateOffset(float x, float z, float val1, float val2){
	float radiansX = ((mod(x+z*x*val1, waveLength)/waveLength) + waveSpeed * mod(x * 0.8 + z, 1.5)) * 2.0 * PI;
	float radiansZ = ((mod(val2 * (z*x +x*z), waveLength)/waveLength) + waveSpeed * 2.0 * mod(x , 2.0) ) * 2.0 * PI;
	return waveAmplitude * 0.5 * (sin(radiansZ) + cos(radiansX));
}

vec3 applyDistortion(vec3 vertex){
	float xDistortion = generateOffset(vertex.x, vertex.z, 0.2, 0.1);
	float yDistortion = generateOffset(vertex.x, vertex.z, 0.1, 0.3);
	float zDistortion = generateOffset(vertex.x, vertex.z, 0.15, 0.2);
	return vertex + vec3(xDistortion, yDistortion, zDistortion);
}

void main()
{
	vec3 currentVertex = vec3(aPos.x,aPos.y,aPos.z);

	vec4 worldPos = model * vec4(currentVertex, 1.0f);
	vs_out.gpass_clipSpaceGrid = projection * view * worldPos;
	
	currentVertex = applyDistortion(currentVertex);
	worldPos = model * vec4(currentVertex, 1.0f);
	
	vs_out.gpass_normal = vec3 (0,1,0);
	
	vs_out.gpass_clipSpaceReal = projection * view * worldPos;
	gl_Position = vs_out.gpass_clipSpaceReal;
	
	vs_out.gpass_toCameraVector = normalize(camPos - currentVertex);
}