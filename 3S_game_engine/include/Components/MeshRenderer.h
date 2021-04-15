#pragma once
#include "Component.h"
#include "Loader/Loader.h"
#include "Material/Material.h"

class MeshRenderer : public Component
{
public:
	Loader::Model model;
	Material material;
	Shader shader;

	MeshRenderer() {}
	MeshRenderer(Loader::Model _model, Shader _shader)
		: model(_model), shader(_shader)
	{}

	void render()
	{
		model.render(shader);
	}
};