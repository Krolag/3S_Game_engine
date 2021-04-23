#include "MeshRenderer.h"
#include "GameLogic/Proctor.h"

namespace GameLogic
{
	MeshRenderer::MeshRenderer(ComponentType _type, Proctor* _proctor, Loader::Model* _model, Shader* _shader)
		: Component(_type, _proctor), parent(proctor), model(_model), shader(_shader)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}

	void MeshRenderer::setModel(Loader::Model* _model)
	{
		model = _model;
	}

	void MeshRenderer::setShader(Shader* _shader)
	{
		shader = _shader;
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
		model->render(*shader);
	}

	void MeshRenderer::cleanup()
	{
		model->cleanup();
	}
}