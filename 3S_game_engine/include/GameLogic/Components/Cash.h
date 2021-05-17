#pragma once
#ifndef CASH_H
#define CASH_H

#include "Component.h"
#include "GameLogic/Hierarchy.h"
#include "Points/Points.h"

namespace GameLogic
{
	class Cash : public Component
	{
	public:
		//Constructor
		Points *points = Points::getInstance();

		Proctor* playerOne;
		Proctor* playerTwo;

		float minDistance = 0.5f;
		float xDistance = 0;
		float zDistance = 0;
		float distance = 0;

		Cash(ComponentType _type, Proctor* _proctor, Proctor* _playerOne, Proctor* _playerTwo);
		~Cash();
		void followPlayer();
		void update();
	};
}

#endif // !CASH_H