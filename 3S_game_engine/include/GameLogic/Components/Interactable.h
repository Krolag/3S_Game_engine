#pragma once
#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "Component.h"

namespace GameLogic
{
	class Interactable : public Component
	{
	public:
		/* Constructor */
		Interactable(ComponentType _type, Proctor* _proctor);
	};
}

#endif // !INTERACTABLE_H
