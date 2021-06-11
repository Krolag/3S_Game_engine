#pragma once
#ifndef CASH_H
#define CASH_H

#include "Component.h"
#include "GameLogic/Hierarchy.h"
#include "Points/Points.h"
#include "../../IrrKlang/irrKlang.h"

namespace GameLogic
{
	class Cash : public Component
	{
	public:
		//Constructor
		Points *points = Points::getInstance();

		float minDistance = 0.5f;
		float xDistance = 0;
		float zDistance = 0;
		float distance = 0;
		float speed = 0.6f;
		bool isFollowing = false;
		irrklang::ISoundEngine* coinSound = irrklang::createIrrKlangDevice();
		irrklang::ISoundSource* coinSource = coinSound->addSoundSourceFromFile("assets/audio/sounds/coin.ogg");

		Cash(ComponentType _type, Proctor* _proctor);
		~Cash();
		void followPlayer();
		void setFollow(bool _isFollowing);
		irrklang::ISoundSource* getCoinSource();
		void update();
	};
}

#endif // !CASH_H