#include "Mesh_rework.h"

bool Mesh::loadMesh(const std::string& _filename)
{
	/* Release the previously loaded mesh */
	clear();

	bool Ret = false;
	Assimp::Importer importer;
	const aiScene* pScene;
	
	if (!noAnim)
	{
		pScene = importer.ReadFile(
			_filename.c_str(),
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs
		);
	}
	else
	{
		pScene = importer.ReadFile(
			_filename.c_str(),
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_PreTransformVertices
		);
	}
	
	if (pScene)
		Ret = initFromScene(pScene, _filename);
	else
		std::cerr << "Error parsing '%s': '%s'\n" << _filename.c_str() << importer.GetErrorString() << std::endl;
	
	return Ret;
}

bool Mesh::initFromScene(const aiScene* _pScene, const std::string& _filename)
{
	entries.resize(_pScene->mNumMeshes);
	textures.resize(_pScene->mNumMaterials);

	/* Initialize the meshes in the scene one by one */
	unsigned int size = entries.size();
	for (unsigned int i = 0; i < size; i++)
	{
		const aiMesh* paiMesh = _pScene->mMeshes[i];
		initMesh(i, paiMesh);
	}

	return initMaterials(_pScene, _filename);
}

void Mesh::initMesh(unsigned int _index, const aiMesh* _paiMesh)
{
	entries[_index].materialIndex = _paiMesh->mMaterialIndex;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	/* Load vertices */
	unsigned int size = _paiMesh->mNumVertices;
	for (unsigned int i = 0; i < size; i++)
	{
		const aiVector3D* pPos = &(_paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(_paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = _paiMesh->HasTextureCoords(0) ? &(_paiMesh->mTextureCoords[0][i]) : &zero3D;

		Vertex v(
			glm::vec3(pPos->x, pPos->y, pPos->z),
			glm::vec3(pNormal->x, pNormal->y, pNormal->z),
			glm::vec2(pTexCoord->x, pTexCoord->y)
		);

		vertices.push_back(v);
	}

	/* Load indices */
	size = _paiMesh->mNumFaces;
	for (unsigned int i = 0; i < size; i++)
	{
		const aiFace& face = _paiMesh->mFaces[i];

		assert(face.mNumIndices == 3);

		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	entries[_index].init(vertices, indices);
}

bool Mesh::initMaterials(const aiScene* _pScene, const std::string& _filename)
{
	/* Extract the directory part from the _filename */
	std::string::size_type slashIndex = _filename.find_last_of("/");
	std::string dir;

	if (slashIndex == std::string::npos)
		dir = ".";
	else if (slashIndex == 0)
		dir = "/";
	else
		dir = _filename.substr(0, slashIndex);

	bool Ret = true;

	/* Initialize the materials */
	unsigned int size = _pScene->mNumMaterials;
	for (unsigned int i = 0; i < size; i++)
	{
		const aiMaterial* pMaterial = _pScene->mMaterials[i];
		textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string fullPath = dir + "/" + path.data;
				textures[i] = new Texture(GL_TEXTURE_2D, fullPath.c_str());

				if (!textures[i]->Load())
				{
					std::cerr << "Error loading texture '%s'\n" << fullPath.c_str() << std::endl;
					delete textures[i];
					textures[i] = NULL;
					Ret = false;
				}
			}
		}
	}

	return Ret;
}
