#include "Interactable.h"
#include "GameLogic/Proctor.h"

namespace GameLogic
{
    Interactable::Interactable(ComponentType _type, Proctor* _proctor)
        : Component(_type, _proctor)
    {
        if (_proctor != NULL)
        {
            proctor->addComponent(this);
        }
    }
}