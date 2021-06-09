#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <assimp/scene.h>
#include <string>

class Texture
{
public:
	/* Constructors */
	Texture(std::string _directory, std::string _path, aiTextureType _type);
	Texture(std::string _directory, std::string _path);
	Texture(std::string _name);
	
	void generate();
	void load(bool _flip = true);
	void bind();
	void allocate(GLenum _format, GLuint _width, GLuint _height, GLenum _type);
	void cleanup();
	static void setParams(
		bool _texFilterSet = false,
		GLenum _texMinFilter = GL_NEAREST,
		GLenum _texMagFilter = GL_NEAREST,
		bool _wrapSet = false,
		GLenum _wrapS = GL_REPEAT,
		GLenum _wrapT = GL_REPEAT
	);

	unsigned int id;
	aiTextureType type;
	std::string name;
	std::string directory;
	std::string path;
};

#endif // !TEXTURE_H