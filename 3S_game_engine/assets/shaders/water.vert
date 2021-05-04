#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 clipSpace;
out vec3 toCamera;
out vec3 fromLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPos;
uniform vec3 lightPos;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0f);
	TexCoords = vec2(aTexCoords.x/2.0 + 0.5,aTexCoords.y/2.0 + 0.5 ) * 2.0;
	clipSpace = projection * view * worldPos;
	toCamera = camPos - worldPos.xyz;
	fromLight = worldPos.xyz - lightPos;
	gl_Position = clipSpace;
}