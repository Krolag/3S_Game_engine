#include "Anima.h"
#include "GameLogic/Proctor.h"
#include "GameLogic/Hierarchy.h"

GameLogic::Anima::Anima(ComponentType _type, Proctor* _proctor, Loader::Model* _model, Shader* _shader)
	: Component(_type, _proctor), animationShader(_shader)
{
	if (_proctor != NULL)
		proctor->addComponent(this);

	playerOneIdle = Animation("./assets/models/players/blue1.fbx", _model);

	idle = new Animator(&playerOneIdle);

	currentAnimator = idle;
}

void GameLogic::Anima::update()
{
	currentAnimator->UpdateAnimation(proctor->getParentHierarchy()->getDeltaTime());

	animationShader->use();
	auto transforms = currentAnimator->GetPoseTransforms();

	for (size_t i = 0; i < transforms.size(); i++)
		animationShader->setUniform("jointTransforms[" + std::to_string(i) + "]", transforms[i]);
}