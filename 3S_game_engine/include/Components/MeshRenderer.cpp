#include "MeshRenderer.h"

MeshRenderer::MeshRenderer(ComponentType _type, Proctor* _proctor)
	: Component(_type, _proctor), parent(proctor)
{ }

void MeshRenderer::setModel(Loader::Model * _model)
{
	model = _model;
}

void MeshRenderer::update()
{
	model->position;
	//Transform transform = proctor->getTransform();
	//model->position = transform.position;
	//model->scale = transform.scale;

	model->render(shader);
}

void MeshRenderer::setShader(Shader _shader)
{
	shader = _shader;
}