#ifndef TEXTURE_H
#include <stb_image.h>
#include <glad/glad.h>

#include <string>

class Texture
{
private:
	unsigned int ID;
	std::string filePath;
	unsigned char* imageDataBuffer;
	int width, height, nrChannels;
	std::string type; // ASK IF IT MEAN TEXTURE_2D/TEXTURE_3D ETC

public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0) const; // Max possible slots are from 0 to 31
	void Unbind();
};
#endif // !TEXTURE_H