#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Shader/Shader.h>
#include <Mesh/Mesh.h>
#include <vector>

class Model
{
public:

	/* Construtor & destructor */
	Model(char *path);
	~Model();

	void DrawModel(Shader &shader);

private:
	/* Model data */
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif // !MODEL_H