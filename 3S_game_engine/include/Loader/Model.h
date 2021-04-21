#pragma once
#ifndef MODEL_H
#define MODEL_H

/* Assimp library*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
/* GLM library */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
/* 3SGE library*/
#include <Shader/Shader.h>
#include "Loader/Mesh.h"
#include "Material/Material.h"
#include <vector>

namespace Loader
{
	class Model
	{
	public:
		std::string path;
		/* Model transformation */
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		Material material;

		/* Construtor / destructor */
		Model(glm::vec3 _position = glm::vec3(0.0f), glm::quat _rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 _scale = glm::vec3(1.0f));

		void init();
		void render(Shader _shader);
		void loadModel(std::string _path);
		void cleanup();

		/* Getters */
		std::vector<Mesh> getMeshes() const;
		glm::mat4 getModelMatrix() const;

	protected:
		/* Model data */
		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> texturesLoaded;

		void processNode(aiNode* _node, const aiScene* _scene);
		Mesh processMesh(aiMesh* _mesh, const aiScene* _scene);

		std::vector<Texture> loadTextures(aiMaterial* _material, aiTextureType _type);
	};
}

#endif // !MODEL_H