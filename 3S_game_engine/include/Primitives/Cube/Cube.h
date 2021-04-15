#pragma once

#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "Camera/Camera.h"

class Cube
{
private:
	Shader cubeShader;
	Texture cubeTexture;
	unsigned int cubeVAO, cubeVBO, cubeEBO; // Vertex array, vertex and element buffer objects
	float cubeVertices[40]; // vertices data
	int cubeIndices[36]; // Indices data
	float cubeSize;

	// need for rendering properly and setting uniforms
	Camera *camera;
	glm::mat4 *model, *view, *projection;

	void init(); // initialization function used in constructors
public:
	// Default constructor, default cube
	Cube(float _cubeSize, Camera * camera, glm::mat4 *model, glm::mat4 *view, glm::mat4 *projection);
	// Shader and Texture from object
	Cube(Shader _cubeShader, Texture _cubeTexture, float _cubeSize, Camera* camera, glm::mat4* model, glm::mat4* view, glm::mat4* projection);
	// Shader and Texture form path
	Cube(std::string _vertexShaderPath, std::string _fragmentShaderPath, std::string _textureDirectory, std::string _texturePath, float _cubeSize, Camera* camera, glm::mat4* model, glm::mat4* view, glm::mat4* projection);

	~Cube();

	void render(); // Draws the cube
};
