#pragma once
#ifndef TREASURE_H
#define TREASURE_H

#include "Component.h"

namespace GameLogic
{
	class Treasure : public Component
	{
	public:
		// Constructor
		Treasure(ComponentType _type, Proctor* _proctor);
	};
}

#endif // !TREASURE_H
