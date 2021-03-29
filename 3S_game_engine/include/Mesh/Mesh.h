#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader/Shader.h>
#include <Texture/Texture.h>
#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

class Mesh
{
public:
	/* Mesh data */
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	std::vector<Vertex> m_vertices;

	/* Constructor & destructor */
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();

	void DrawMesh(Shader &shader);

private:
	/* Render data */
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};

#endif // !MESH_H