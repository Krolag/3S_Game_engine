#include "Treasure.h"
#include "GameLogic/Proctor.h"

namespace GameLogic
{
	Treasure::Treasure(ComponentType _type, Proctor* _proctor) : Component(_type, _proctor)
	{
		if (_proctor != NULL)
		{
			proctor->addComponent(this);
		}
	}
}