#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <assimp/scene.h>
#include <string>

class Texture
{
public:
	/* Constructors */
	Texture();
	Texture(std::string _directory, std::string _path, aiTextureType _type);
	Texture(std::string _directory, std::string _path);
	
	void generate();
	void load(bool _flip = true);
	void bind(int _slot = 0);

	unsigned int id;
	aiTextureType type;
	std::string directory;
	std::string path;
};

#endif // !TEXTURE_H