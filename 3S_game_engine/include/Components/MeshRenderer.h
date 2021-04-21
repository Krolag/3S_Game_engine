#pragma once
#include "Component.h"
#include "Loader/Loader.h"
#include "Material/Material.h"

class MeshRenderer : public Component
{
public:
	/* Constructors */
	MeshRenderer(ComponentType _type, Proctor* _proctor);

	/* Model methods */
	void setModel(Loader::Model* _model);
	void setShader(Shader _shader);
	
	void update();
	void cleanup();

private:
	Proctor* parent = nullptr;
	Loader::Model* model = nullptr;
	Shader shader;
};