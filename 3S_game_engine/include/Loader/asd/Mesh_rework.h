#pragma once
#ifndef MESH_H
#define MESH_H

#include <map>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
/* Assimp library*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/AssimpGLMHelpers.h>
/* GLM library */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	Vertex() {}

	Vertex(const glm::vec3 _position, glm::vec3 _normal, glm::vec2 _texCoord)
	{
		position = _position;
		normal = _normal;
		texCoord = _texCoord;
	}
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool loadMesh(const std::string& _filename);
	void render();

protected:
	bool noAnim = true;

private:
	bool initFromScene(const aiScene* _pScene, const std::string& _filename);
	void initMesh(unsigned int _index, const aiMesh* _paiMesh);
	bool initMaterials(const aiScene* _pScene, const std::string& _filename);
	void clear();

#define INVALID_MATERIAL 0xFFFFFFFF
	
	struct MeshEntry
	{
		MeshEntry();
		~MeshEntry();

		bool init(const std::vector<Vertex>& _vertices, const std::vector<unsigned int>& _indices);

		GLuint VB;
		GLuint IB;
		unsigned int numIndices;
		unsigned int materialIndex;
	};

	std::vector<MeshEntry> entries;
	std::vector<Texture*> textures;
};

#endif // !MESH_H
