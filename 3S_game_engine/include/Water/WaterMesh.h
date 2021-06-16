#pragma once
#include <Shader/Shader.h>
class WaterMesh
{
public:
	glm::vec3 waterColor = glm::vec3(0.4f, 0.8f, 0.9f);

	WaterMesh(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath, int VertexCount, int size);
	~WaterMesh();
	void render(glm::mat4 model, glm::mat4 projection, glm::mat4 view, int reflectionTex, float time, glm::vec3 cameraPos);
	void loadTexture(std::string texturePath, std::string normalMapPath);
	void generateMesh(int vertexCount, int size);

private:

	Shader waterShader;
	unsigned int VAO, VBO, VBO_Texture,EBO;
	unsigned int reflectionTexture;

	float* vertices;
	float* textureCoords;
	int* indices;

	int indicesArraySize;
	int textureArraySize;
	int verticesArraySize;

	int vertexCountSquared;

	const float WATER_SPEED = 0.15;
	float waveSpeed = 0;
};

