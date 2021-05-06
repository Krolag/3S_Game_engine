#include "MeshRenderer.h"
#include "GameLogic/Proctor.h"
#include "GameLogic/Hierarchy.h"
#include "Camera/FrustumCulling.h"

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
		/* Check if proctor is in camera view */
		if (proctor->getComponentOfType(C_COLLIDER) != NULL)
		{
			if (FrustumCulling::boxAABBInFrustum(
				proctor->transform.position.x,
				proctor->transform.position.y,
				proctor->transform.position.z,
				((BoxCollider*)proctor->getComponentOfType(C_COLLIDER))->getRadius().x,
				proctor->getParentHierarchy()->getCamera()
			))
			{
				model->render(*shader);
			}
		}
		// TODO: @Dawid - gdy beda collidery, zmien to
		/* temporary solution for proctors without colliders */
		else
		{
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
