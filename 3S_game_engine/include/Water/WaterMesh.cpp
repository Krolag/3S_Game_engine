#include "WaterMesh.h"
#include <iostream>
#include <stb_image.h>

WaterMesh::WaterMesh(std::string vertexShaderPath, std::string fragmentShaderPath, std::string dudvTexturePath, std::string normalTexturePath, int vertexCount,int size) :waterShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str()), VAO(0), VBO(0), VBO_Texture(0),EBO(0)

{
	
	generateMesh(vertexCount, size);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); //vbo buffer for vertices 
	glGenBuffers(1, &VBO_Texture); //vbo buffer for textureCords 
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	//bind vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesArraySize * sizeof(float), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	//bind textures
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Texture);
	glBufferData(GL_ARRAY_BUFFER, textureArraySize * sizeof(float), textureCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	//bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesArraySize * sizeof(float), indices, GL_STATIC_DRAW);

	glGenTextures(1, &reflectionTexture);
	glBindTexture(GL_TEXTURE_2D, reflectionTexture);

	glGenTextures(1, &refractionTexture);
	glBindTexture(GL_TEXTURE_2D, refractionTexture);

	waterShader.use();
	waterShader.setUniformInt("reflectionTexture", 0);
	waterShader.setUniformInt("refractionTexture", 1);

	loadTexture(dudvTexturePath,normalTexturePath);
}

void WaterMesh::render(glm::mat4 model, glm::mat4 projection, glm::mat4 view, int reflectionTex, int refractionTex, float time,glm::vec3 cameraPos)
{
	waveSpeed += WATER_SPEED * time;
	if (waveSpeed > 1) waveSpeed = 0.f;
	waterShader.use();
	waterShader.setUniform("model", model);
	waterShader.setUniform("projection", projection);
	waterShader.setUniform("view", view);
	waterShader.setUniformFloat("waveSpeed", waveSpeed);
	waterShader.setUniform("camPos", cameraPos);
	waterShader.setUniform("lightPos", cameraPos.x, cameraPos.y, cameraPos.z);
	waterShader.setUniform("lightColor", 0.8f, 0.8f, 0.8f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, reflectionTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, refractionTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudvMap);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalMap);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, indicesArraySize, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void WaterMesh::loadTexture(std::string dudvMapPath, std::string normalMapPath)
{
	glGenTextures(1, &dudvMap);
	glBindTexture(GL_TEXTURE_2D, dudvMap);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(dudvMapPath.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, dudvMap);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << dudvMapPath.c_str() << std::endl;
		stbi_image_free(data);
	}

	//normalmap
	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);

	data = stbi_load(normalMapPath.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << dudvMapPath.c_str() << std::endl;
		stbi_image_free(data);
	}

	waterShader.use();
	waterShader.setUniformInt("dudvMap", 2);
	waterShader.setUniformInt("normalMap", 3);
}

void WaterMesh::generateMesh(int vertexCount,int size) //vertexCount - number of vertices per side , size - scale
{
	vertexCountSquared = vertexCount * vertexCount;
	verticesArraySize = vertexCountSquared * 3;
	textureArraySize = vertexCountSquared * 2;
	indicesArraySize = 6 * (vertexCount - 1) * (vertexCount - 1);
	vertices = new float[verticesArraySize];
	textureCoords = new float[textureArraySize];

	indices = new int[indicesArraySize];
	int vertexPointer = 0;
	for (int i = 0; i < vertexCount; i++) {
		for (int j = 0; j < vertexCount; j++) {
			vertices[vertexPointer * 3] = (float)j / ((float)vertexCount - 1) * size;
			vertices[vertexPointer * 3 + 1] = 0;
			vertices[vertexPointer * 3 + 2] = (float)i / ((float)vertexCount - 1) * size;

			textureCoords[vertexPointer * 2] = (float)j / ((float)vertexCount - 1);
			textureCoords[vertexPointer * 2 + 1] = (float)i / ((float)vertexCount - 1);
			vertexPointer++;
		}
	}
	int pointer = 0;
	for (int g = 0; g < vertexCount - 1; g++) {
		for (int h = 0; h < vertexCount - 1; h++) {
			int topLeft = (g * vertexCount) + h;
			int topRight = topLeft + 1;
			int bottomLeft = ((g + 1) * vertexCount) + h;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}
}

WaterMesh::~WaterMesh()
{
	delete vertices;
	delete textureCoords;
	delete indices;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}