#pragma once
#ifndef ANIMA_H
#define ANIMA_H

#include "Component.h"
#include "AnimationSystem/System/Animator.h"
#include "Loader/Model.h"
#include "Shader/Shader.h"

namespace GameLogic
{
	
	
	class Anima : public Component
	{
	public:
		Animator* currentAnimator;
		Animator* idle;

		Animation playerOneIdle;
		Shader* animationShader;

		Anima(ComponentType _type, Proctor* _proctor, Loader::Model* _model, Shader* _shader);

		void update();
	};
}

#endif // !ANIMA_H
