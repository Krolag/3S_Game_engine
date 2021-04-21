#include "MeshRenderer.h"
#include "GameLogic/Proctor.h"

MeshRenderer::MeshRenderer(ComponentType _type, Proctor* _proctor)
	: Component(_type, _proctor), parent(proctor)
{ }

void MeshRenderer::setModel(Loader::Model * _model)
{
	model = _model;
}

void MeshRenderer::update()
{
	/* Create transform container for proctor's transform values */
	Transform transform = proctor->getTransform();

	/* Apply those values each frame */
	model->position = transform.position;
	model->rotation = transform.rotation;
	model->scale = transform.scale;

	/* Render model with given shader */
	model->render(shader);
}

void MeshRenderer::cleanup()
{
	model->cleanup();
}

void MeshRenderer::setShader(Shader _shader)
{
	shader = _shader;
}