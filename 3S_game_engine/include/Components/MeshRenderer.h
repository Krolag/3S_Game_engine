#pragma once
#include "Component.h"
#include "Loader/Loader.h"
#include "Material/Material.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(ComponentType _type)
		: Component(_type, NULL)
	{

	}

	MeshRenderer(ComponentType _type, Proctor* _proctor);

	void setModel(Loader::Model* _model)
	{
		model = _model;
	}

	void update()
	{
		model->render(shader);
	}

	void setShader(Shader _shader)
	{
		shader = _shader;
	}

private:
	Loader::Model* model = nullptr;
	Shader shader;
};