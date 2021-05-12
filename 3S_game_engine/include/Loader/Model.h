#pragma once
#ifndef MODEL_H
#define MODEL_H

/* Assimp library*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/AssimpGLMHelpers.h>
/* GLM library */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
/* 3SGE library*/
#include <Shader/Shader.h>
#include "Loader/Mesh.h"
#include "Material/Material.h"
#include <vector>
#include <map>

namespace Loader
{
#define MAX_BONE_WEIGHTS 4

	struct BoneInfo
	{
		/* ID is index in finalBoneMatrices */
		int id;
		/* Offset matrix transforms vertex from model space to bone space */
		glm::mat4 offset;
	};

	class Model
	{
	public:
		std::string path;
		std::string name;
		/* Model transformation */
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		

		/* Construtor / destructor */
		Model(std::string _path, std::string _name, bool _noTex = false, glm::vec3 _position = glm::vec3(0.0f), glm::quat _rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 _scale = glm::vec3(1.0f));
		Model(bool _noTex = false, glm::vec3 _position = glm::vec3(0.0f), glm::quat _rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 _scale = glm::vec3(1.0f));

		void init();
		void render(Shader _shader);
		void loadModel(std::string _path);
		void cleanup();

		/* Getters */
		std::vector<Mesh> getMeshes() const;
		glm::mat4 getModelMatrix() const;
		auto& getBoneInfoMap() { return boneInfoMap; }
		int& getBoneCounter() { return boneCounter; }

	protected:
		/* Model data */
		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> texturesLoaded;
		std::map<std::string, BoneInfo> boneInfoMap;
		int boneCounter = 0;
		bool noTex;

		void processNode(aiNode* _node, const aiScene* _scene);
		Mesh processMesh(aiMesh* _mesh, const aiScene* _scene);
		void setVertexBoneDataToDefault(Vertex& _vertex);
		void setVertexBoneData(Vertex& _vertex, int _boneID, float _weight);
		void extractBoneWeightForVertices(std::vector<Vertex>& _vertices, aiMesh* _mesh, const aiScene* _scene);

		std::vector<Texture> loadTextures(aiMaterial* _material, aiTextureType _type);
	};
}

#endif // !MODEL_H