#pragma once
#include <Shader/Shader.h>
class WaterMesh
{
public:

	WaterMesh(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath, int VertexCount, int size);
	~WaterMesh();
	void render(glm::mat4 model, glm::mat4 projection, glm::mat4 view);
	void loadTexture(std::string texturePath);	
	void generateMesh(int vertexCount, int size);

private:

	Shader waterShader;
	unsigned int VAO, VBO, VBO_Texture,EBO;
	unsigned int texture1;

	float* vertices;
	float* textureCoords;
	int* indices;

	int indicesArraySize;
	int textureArraySize;
	int verticesArraySize;

	int vertexCountSquared;

};

