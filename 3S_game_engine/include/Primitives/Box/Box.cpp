/**
 * @class	Box
 * @brief	Class responsible for generating and rendering boxes
 *
 * @author	Kuba Podkomórka 224407
 */

#include "Box.h"

// make it one collor basic cube later
Box::Box() : boxVAO(0), boxVBO(0), transform(glm::mat4(1.0f)),
	boxShader("assets/shaders/basic.vert", "assets/shaders/basic.frag"),
	boxTexture("assets/textures", "wall.jpg")
{
	init();
}

Box::Box(Shader _boxShader, std::string _boxTextureDirectory, std::string _boxTexturePath) :
	boxVAO(0), boxVBO(0), transform(glm::mat4(1.0f)),
	boxShader(_boxShader), boxTexture(_boxTextureDirectory, _boxTexturePath)
{
	init();
}

Box::Box(Shader _boxShader, Texture _boxTexture) : boxVAO(0), boxVBO(0), transform(glm::mat4(1.0f)),
	boxShader(_boxShader), boxTexture(_boxTexture)
{
	init();
}

Box::Box(Shader _boxShader, glm::vec3 min, glm::vec3 max) : boxVAO(0), boxVBO(0), transform(glm::mat4(1.0f)),
	boxShader(_boxShader),
	boxTexture("assets/textures", "greentexture.jpg")
{
	init(true, min, max);
}

Box::~Box()
{
	//model = nullptr;
}

void Box::init(bool minMaxConstructor, glm::vec3 min, glm::vec3 max)
{
	/* init vertices data */
	if (minMaxConstructor) fillBoxMinMaxData(min, max);
	else fillBoxVertData();

	/* generate vertex array and vertex buffer */
	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);

	/* bind generated array to use it */
	glBindVertexArray(boxVAO);

	/* vertex buffer data */
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), &boxVertices, GL_STATIC_DRAW);

	/* vertex array data on specific position for vertices */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	/* vertex array data on specific position for texture coords */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// load texture
	boxTexture.load(false);
}

void Box::render(bool renderWithLines)
{
	boxShader.use(); // we want to use shader belonging to this object
	boxShader.setUniform("model", transform);
	glEnable(GL_DEPTH_TEST);
	
	glBindVertexArray(boxVAO); // bind vertex array belonging to this object

	boxTexture.bind(); // bind ui texture to use it

	// Draw call with triangles or lines ( lines are useful for the collider box rendering)
	if (renderWithLines) glDrawArrays(GL_LINE_STRIP, 0, 36);
	else glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0); // unbind vertex array belonging to this object
}

void Box::translate(glm::vec3 translation)
{
	transform = glm::translate(transform, translation);
}

void Box::rotate(float degrees, glm::vec3 rotation)
{
	transform = glm::rotate(transform, degrees, rotation);
}

void Box::scale(glm::vec3 scale)
{
	transform = glm::scale(transform, scale);
}

// GETTERS

float* Box::getBoxVertices()
{
	return boxVertices;
}

Shader Box::getShader()
{
	return boxShader;
}

glm::mat4 Box::getTransform()
{
	return transform;
}

// SETTERS

void Box::setTransform(glm::mat4 _transform)
{
	transform = glm::mat4(_transform);
}

// THE LAST IS METHODS ARE FILLING VERTICES TABLE WITH DATA, JUST FOR THE VISUAL SAKE THEY'RE LAST

void Box::fillBoxVertData()
{
	float verts[] = {
		// FRONT
		// x,	 y,		z,	   s,	  t
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,

		// BACK
		-0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		
		// LEFT
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		
		// RIGHT
		 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		
		// TOP
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,
		
		// BOTTOM
		-0.5f, -0.5f, -0.5f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  1.0f,  1.0f,
	};
	for (int i = 0; i < 180; i++)
	{
		boxVertices[i] = verts[i];
	}
}

void Box::fillBoxMinMaxData(glm::vec3 min, glm::vec3 max)
{
	float verts[] = {
		// FRONT
		min.x, max.y, max.z, 0.0f,  1.0f,
		max.x, max.y, max.z, 1.0f,  1.0f,
		max.x, min.y, max.z, 1.0f,  0.0f,
		max.x, min.y, max.z, 1.0f,  0.0f,
		min.x, min.y, max.z, 0.0f,  0.0f,
		min.x, max.y, max.z, 0.0f,  1.0f,

		// BACK
		min.x, max.y, min.z, 1.0f,  1.0f,
		max.x, max.y, min.z, 0.0f,  1.0f,
		max.x, min.y, min.z, 0.0f,  0.0f,
		max.x, min.y, min.z, 0.0f,  0.0f,
		min.x, min.y, min.z, 1.0f,  0.0f,
		min.x, max.y, min.z, 1.0f,  1.0f,

		// LEFT
		min.x, max.y, min.z, 0.0f,  1.0f,
		min.x, max.y, max.z, 1.0f,  1.0f,
		min.x, min.y, max.z, 1.0f,  0.0f,
		min.x, min.y, max.z, 1.0f,  0.0f,
		min.x, min.y, min.z, 0.0f,  0.0f,
		min.x, max.y, min.z, 0.0f,  1.0f,

		// RIGHT
		max.x, max.y, min.z, 1.0f,  1.0f,
		max.x, max.y, max.z, 0.0f,  1.0f,
		max.x, min.y, max.z, 0.0f,  0.0f,
		max.x, min.y, max.z, 0.0f,  0.0f,
		max.x, min.y, min.z, 1.0f,  0.0f,
		max.x, max.y, min.z, 1.0f,  1.0f,

		// TOP
		min.x, max.y, min.z, 0.0f,  1.0f,
		max.x, max.y, min.z, 1.0f,  1.0f,
		max.x, max.y, max.z, 1.0f,  0.0f,
		max.x, max.y, max.z, 1.0f,  0.0f,
		min.x, max.y, max.z, 0.0f,  0.0f,
		min.x, max.y, min.z, 0.0f,  1.0f,

		// BOTTOM			 
		min.x, min.y, min.z, 1.0f,  1.0f,
		max.x, min.y, min.z, 0.0f,  1.0f,
		max.x, min.y, max.z, 0.0f,  0.0f,
		max.x, min.y, max.z, 0.0f,  0.0f,
		min.x, min.y, max.z, 1.0f,  0.0f,
		min.x, min.y, min.z, 1.0f,  1.0f,
	};
	for (int i = 0; i < 180; i++)
	{
		boxVertices[i] = verts[i];
	}
}