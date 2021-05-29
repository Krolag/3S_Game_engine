#pragma once
#ifndef ANIMA_H
#define ANIMA_H

#include "Component.h"
#include "AnimationSystem/System/Animation.h"
#include "AnimationSystem/System/Animator.h"
#include "GameLogic/Proctor.h"
#include "Loader/Model.h"

namespace GameLogic
{
	class Anima : public Component
	{
	public:
		Animator animator;
		std::vector<Animation> loadedAnimations;

		/* Constructors */
		Anima(ComponentType _type, Proctor* _proctor = NULL);

		void update();
		void playAnimation(int _index);

	protected:
		int cIndex;

	private:
		Loader::Model* model;
		void getModelFromProctor();
		void initializeAnimations();
	};
}

#endif // !ANIMA_H
