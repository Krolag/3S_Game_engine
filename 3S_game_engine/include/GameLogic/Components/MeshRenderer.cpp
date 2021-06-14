#include "MeshRenderer.h"
#include "GameLogic/Proctor.h"
#include "GameLogic/Hierarchy.h"

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
		model->parentPosition = transform.parentPosition;
		model->parentRotation = transform.parentRotation;
		model->scale = transform.scale;

		if (proctor->getParentProctor() != NULL)
		{
			shader->use();
			shader->setUniformBool("noAnim", model->getNoAnim());
			model->renderChild(*shader);
		}
		else {
			shader->use();
			shader->setUniformBool("noAnim", model->getNoAnim());
			model->render(*shader);
		}
	}

	void MeshRenderer::cleanup()
	{
		model->cleanup();
	}

	void MeshRenderer::assignToProctor(Proctor* _proctor)
	{
		proctor->addComponent(this);
	}
}