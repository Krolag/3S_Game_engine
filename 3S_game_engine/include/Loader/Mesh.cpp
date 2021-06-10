#include "Mesh.h"

namespace Loader
{
	std::vector<Vertex> Vertex::genList(float* _vertices, int _noVertices)
	{
		std::vector<Vertex> ret(_noVertices);

		/* Calculate stride */
		int stride = sizeof(Vertex) / sizeof(float);

		for (int i = 0; i < _noVertices; i++)
		{
			ret[i].position = glm::vec3(
				_vertices[i * stride + 0],
				_vertices[i * stride + 1],
				_vertices[i * stride + 2]
			);

			ret[i].normal = glm::vec3(
				_vertices[i * stride + 3],
				_vertices[i * stride + 4],
				_vertices[i * stride + 5]
			);

			ret[i].texCoord = glm::vec2(
				_vertices[i * stride + 6],
				_vertices[i * stride + 7]
			);
		}

		return ret;
	}


	Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures)
		: vertices(_vertices), indices(_indices), textures(_textures), noTex(false)
	{
		setup();
	}

	Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, aiColor4D _diffuse, aiColor4D _specular)
		: vertices(_vertices), indices(_indices), diffuse(_diffuse), specular(_specular), noTex(true)
	{ 
		setup();
	}

	void Mesh::render(Shader shader)
	{
		if (noTex)
		{
			/* Materials */
			shader.setUniform("material.diffuse", diffuse);
			shader.setUniform("material.specular", specular);
			shader.setUniformInt("noTex", 1);
		}
		else
		{
			/* Textures */
			unsigned int diffuseIdx = 0;
			unsigned int specularIdx = 0;

			for (unsigned int i = 0; i < textures.size(); i++)
			{
				/* Activate textures */
				glActiveTexture(GL_TEXTURE + i);
				/* Retrive texture info */
				std::string name;
				switch (textures[i].type)
				{
				case aiTextureType_DIFFUSE:
					name = "diffuse" + std::to_string(diffuseIdx++);
					break;
				case aiTextureType_SPECULAR:
					name = "specular" + std::to_string(specularIdx++);
					break;
				}

				/* Set texture value in the shader */
				shader.setUniformInt(name, i);
				textures[i].bind();
			}

			shader.setUniformInt("noTex", 0);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}

	void Mesh::cleanup()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

	void Mesh::setup()
	{
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		
		/* Load data */
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		/* Set vertex attribute pointers */
		/* Vertex.position */
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		/* Vertex.normal */
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		/* Vertex.texCoord */
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		/* Vertex.boneIDs*/
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
		//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));
		/* Vertex.weights */
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}