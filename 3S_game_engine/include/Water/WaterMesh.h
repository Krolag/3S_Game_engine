#pragma once
#include <Shader/Shader.h>
class WaterMesh
{
public:

	WaterMesh(std::string vertexShaderPath, std::string fragmentShaderPath, std::string geometryShaderPath, int VertexCount, int size);
	~WaterMesh();
	void render(glm::mat4 model, glm::mat4 projection, glm::mat4 view, int reflectionTex, int refractionTex, float time, glm::vec3 cameraPos);
	void loadTexture(std::string texturePath, std::string normalMapPath);
	void generateMesh(int vertexCount, int size);

private:

	Shader waterShader;
	unsigned int VAO, VBO, VBO_Texture,EBO;
	unsigned int reflectionTexture, refractionTexture;

	float* vertices;
	float* textureCoords;
	int* indices;

	int indicesArraySize;
	int textureArraySize;
	int verticesArraySize;

	int vertexCountSquared;

	const float WATER_SPEED = 0.4;
	float waveSpeed = 0;
};

