#include "Cube.h"

Cube::Cube(float _cubeSize, Camera* _camera, glm::mat4* _model, glm::mat4* _view, glm::mat4* _projection) : cubeVAO(0), cubeVBO(0), cubeEBO(0), cubeSize(_cubeSize),
	cubeShader("assets/shaders/basic.vert", "assets/shaders/basic.frag"),
	cubeTexture("assets/textures", "wall.jpg"),
	camera(_camera), model(_model), view(_view), projection(_projection)
{
	init();
}

Cube::Cube(Shader _cubeShader, Texture _cubeTexture, float _cubeSize, Camera* _camera, glm::mat4* _model, glm::mat4* _view, glm::mat4* _projection) : cubeVAO(0), cubeVBO(0), cubeEBO(0),
	cubeShader(_cubeShader), cubeTexture(_cubeTexture), cubeSize(_cubeSize),
	camera(_camera), model(_model), view(_view), projection(_projection)
{
	init();
}

Cube::Cube(std::string _vertexShaderPath, std::string _fragmentShaderPath, std::string _textureDirectory, std::string _texturePath, float _cubeSize, Camera* _camera, glm::mat4* _model, glm::mat4* _view, glm::mat4* _projection) :
	cubeShader(_vertexShaderPath.c_str(), _fragmentShaderPath.c_str()),
	cubeTexture(_textureDirectory, _texturePath),
	cubeVAO(0), cubeVBO(0), cubeEBO(0), cubeSize(_cubeSize),
	camera(_camera), model(_model), view(_view), projection(_projection)
{
	init();
}

Cube::~Cube()
{
}

void Cube::init()
{
	// for shorter and less messy data table below (instead of drawing everywhere 0.5 * cubeSize we can assign halfsize from the start)
	float halfSize = cubeSize / 2.0f;

	// init vertices data
	float _cubeVert[] = {
		//	 x,			y,		  z,     s,    t
		-halfSize,  halfSize, halfSize, 0.0f, 1.0f,	// front
		 halfSize,  halfSize, halfSize, 1.0f, 1.0f,
		 halfSize, -halfSize, halfSize, 1.0f, 0.0f,
		-halfSize, -halfSize, halfSize, 0.0f, 0.0f,
		
		-halfSize,  halfSize, -halfSize, 1.0f, 1.0f, // back
		 halfSize,  halfSize, -halfSize, 0.0f, 1.0f,
		 halfSize, -halfSize, -halfSize, 0.0f, 0.0f,
		-halfSize, -halfSize, -halfSize, 1.0f, 0.0f,
	};
	
	for (int i = 0; i < 40; i++)
	{
		cubeVertices[i] = _cubeVert[i];
	}

	// init indices data
	float _cubeInd[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		
		// back
		4, 5, 6,
		6, 7, 4,

		// left
		0, 4, 7,
		7, 3, 0,
		
		// right
		1, 5, 6,
		6, 2, 1,
		
		// top
		0, 4, 5,
		5, 1, 0,
		
		// bottom
		3, 7, 6,
		6, 2, 3
	};
	
	for (int i = 0; i < 36; i++)
	{
		cubeIndices[i] = _cubeInd[i];
	}

	// generate vertex array following vertex and element buffers
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);

	// vertex buffer data
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	// element buffer data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	// vertex array data on specific position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// load texture
	cubeTexture.load(false);
}

void Cube::render()
{
	cubeShader.use(); // we want to use shader belonging to this object
	cubeShader.setUniform("model", *model);
	cubeShader.setUniform("view", *view);
	cubeShader.setUniform("projection", *projection);
	glEnable(GL_DEPTH_TEST);
	
	glBindVertexArray(cubeVAO); // bind vertex array belonging to this object

	cubeTexture.bind(); // bind ui texture to use it
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // draw call

	glBindVertexArray(0); // unbind vertex array belonging to this object
}