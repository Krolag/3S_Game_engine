#ifndef TEXTURE_H
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

	void Bind() const;
	void Unbind();

	void Active(unsigned int slot = 0) const; // Max possible slots are from 0 to 31

	unsigned int GetID();
	std::string GetType();
};
#endif // !TEXTURE_H