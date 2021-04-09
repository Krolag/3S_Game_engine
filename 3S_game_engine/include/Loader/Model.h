#ifndef MODEL_H
#define MODEL_H
/* Assimp library*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
/* GLM library */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
/* 3SGE library*/
#include <Shader/Shader.h>
#include "Loader/Mesh.h"
#include <vector>

namespace Loader
{
	class Model
	{
	public:
		/* Model transformation */
		glm::vec3 position;
		glm::vec3 size;

		/* Construtor & destructor */
		Model(glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _size = glm::vec3(1.0f));

		void init();
		void render(Shader _shader);
		void loadModel(std::string _path);
		void cleanup();

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