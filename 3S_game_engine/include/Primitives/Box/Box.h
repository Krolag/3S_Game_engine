/**
 * @class	Box
 * @brief	Class responsible for generating and rendering boxes
 *
 * @author	Kuba Podkomórka 224407
 */

#pragma once

#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "Camera/Camera.h"

class Box
{
private:
	Shader boxShader;
	Texture boxTexture;

	unsigned int boxVAO, boxVBO;
	float boxVertices[180];
	
	glm::mat4 transform;

	/* Initialization methods */
	void init(bool minMaxConstructor = false, glm::vec3 min = glm::vec3(1.f), glm::vec3 max = glm::vec3(1.f));
	void fillBoxVertData();
	void fillBoxMinMaxData(glm::vec3 min, glm::vec3 max);
public:
	/* Constructors */
	Box();
	Box(Shader _boxShader, std::string _boxTextureDirectory, std::string _boxTexturePath);
	Box(Shader _boxShader, Texture _boxTexture);
	Box(Shader _boxShader, glm::vec3 min, glm::vec3 max); // For the box collider

	/* Destructor */
	~Box();

	/* Main class methods */
	void render(bool renderWithLines = false); // Draws the cube with triangles or lines

	/* Transform methods */
	void translate(glm::vec3 translate);
	void rotate(float degrees, glm::vec3 axis);
	void scale(glm::vec3 scale);

	/* Getters */
	float* getBoxVertices();
	Shader getShader();
	glm::mat4 getTransform();

	/* Setters */
	void setTransform(glm::mat4 _transform);
};