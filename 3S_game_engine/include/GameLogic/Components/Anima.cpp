#include "Anima.h"

namespace GameLogic
{
	Anima::Anima(ComponentType _type, Proctor* _proctor)
		: Component(_type, _proctor)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}

		getModelFromProctor();
		initializeAnimations();
	}

	void Anima::update()
	{
		/* Apply all bones transforms */
		((MeshRenderer*)proctor->getComponentOfType(C_MESH))->shader->use();
		auto transforms = animator.getFinalBoneMatrices();
		int size = transforms.size();
		for (int i = 0; i < size; i++)
			((MeshRenderer*)proctor->getComponentOfType(C_MESH))->shader->setUniform("jointTransforms[" + std::to_string(i) + "]", transforms[i]);

		/* Update animator */
		animator.updateAnimation(proctor->getDeltaTime());
	}

	void Anima::playAnimation(int _index)
	{
		if (_index != cIndex)
		{
			cIndex = _index;
			animator.playAnimation(&loadedAnimations[_index]);
		}
	}

	void Anima::getModelFromProctor()
	{
		MeshRenderer* renderer = ((MeshRenderer*)proctor->getComponentOfType(C_MESH));

		if (renderer != NULL)
		{
			this->model = renderer->model;
		}
		else
			std::cerr << "CAN'T FIND ANY MODEL FOR THIS PROCTOR: " << proctor->name << std::endl;
	}

	void Anima::initializeAnimations()
	{
		int size = this->model->scene->mNumAnimations;
		for (int i = 0; i < size; i++)
		{
			this->loadedAnimations.push_back(
				Animation(
					this->model->path.c_str(), 
					this->model, 
					i
				));
		}

		if (size != 0)
		{
			cIndex = 0;
			animator.playAnimation(&loadedAnimations[0]);
		}
	}
}