#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <Shader/Shader.h>
#include <Texture/Texture.h>

namespace Loader
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;

		static std::vector<Vertex> genList(float* _vertices, int _noVertices);
	};

	class Mesh
	{
	public:
		/* Mesh data */
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		aiColor4D diffuse;
		aiColor4D specular;
		unsigned int VAO;

		/* Constructors */
		Mesh();
		Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures);
		Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, aiColor4D _diffuse, aiColor4D _specular);

		void render(Shader shader);
		void cleanup();

	private:
		unsigned int VBO, EBO;
		bool noTex;
		void setup();
	};
}
#endif // !MESH_H