#include "Model.h"

namespace Loader
{
	Model::Model(glm::vec3 _position, glm::vec3 _size)
		: position(_position), size(_size) {}

	void Model::init() {}

	void Model::render(Shader _shader)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->position);
		model = glm::scale(model, this->size);
		_shader.setUniform("model", model);
		_shader.setUniform("material.shininess", 0.5f);

		for (Mesh mesh : meshes)
		{
			mesh.render(_shader);
		}
	}

	void Model::loadModel(std::string _path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_path,
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_PreTransformVertices);

		/* Check if scene is imported properly */
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Failed to load model at " << _path << std::endl << "Reason: " << importer.GetErrorString() << std::endl;
			return;
		}

		this->directory = _path.substr(0, _path.find_last_of("/"));
		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* _node, const aiScene* _scene)
	{
		/* Process all the meshes */
		for (unsigned int i = 0; i < _node->mNumMeshes; i++)
		{
			aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, _scene));
		}

		/* Process all child nodes */
		for (unsigned int i = 0; i < _node->mNumChildren; i++)
		{
			processNode(_node->mChildren[i], _scene);
		}
	}

	Mesh Model::processMesh(aiMesh* _mesh, const aiScene* _scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		/* Process Vertices */
		for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
		{
			Vertex vertex;

			/* Position */
			vertex.position = glm::vec3(
				_mesh->mVertices[i].x,
				_mesh->mVertices[i].y,
				_mesh->mVertices[i].z
			);
			/* Normal vectors */
			vertex.normal = glm::vec3(
				_mesh->mNormals[i].x,
				_mesh->mNormals[i].y,
				_mesh->mNormals[i].z
			);
			/* Textures */
			if (_mesh->mTextureCoords[0])
			{
				vertex.texCoord = glm::vec2(
					_mesh->mTextureCoords[0][i].x,
					_mesh->mTextureCoords[0][i].y
				);
			}
			else
			{
				vertex.texCoord = glm::vec2(1.0f);
			}

			/* Push completed data */
			vertices.push_back(vertex);
		}

		/* Process Indices */
		for (unsigned int i = 0; i < _mesh->mNumFaces; i++)
		{
			aiFace face = _mesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		/* Process Material */
		if (_mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];

			/* Diffuse maps */
			std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			/* Specular maps */
			std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		/* Return properly collected data */
		return Mesh(vertices, indices, textures);
	}

	std::vector<Texture> Model::loadTextures(aiMaterial* _material, aiTextureType _type)
	{
		std::vector<Texture> textures;

		for (unsigned int i = 0; i < _material->GetTextureCount(_type); i++)
		{
			aiString str;
			_material->GetTexture(_type, i, &str);

			/* Prevent duplicate loading */
			bool skip = false;

			for (unsigned int j = 0; j < texturesLoaded.size(); j++)
			{
				if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(texturesLoaded[j]);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				/* Not loaded yet */
				Texture tex(this->directory, str.C_Str(), _type);
				tex.load(false); // we don't want to flip it, flipUV's is doing it for us
				textures.push_back(tex);
				texturesLoaded.push_back(tex);
			}
		}

		return textures;
	}

	void Model::cleanup()
	{
		for (Mesh mesh : meshes)
		{
			mesh.cleanup();
		}
	}
}