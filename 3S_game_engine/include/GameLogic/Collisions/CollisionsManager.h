/**
 * @class	CollisionsManager
 * @brief	Class stores all of the colliders and deals with the collisions
 *
 * @author	Kuba Podkomórka 224407
 */

#pragma once

#include <vector>

#include "Collider.h"

class CollisionsManager
{
private:
	std::vector<Collider> colliders;
public:
	/* Constructors */
	CollisionsManager();
	CollisionsManager(std::vector<Collider> _colliders);

	/* Main class methods */
	void Add(Collider _collider);
	void Remove(Collider _collider);
};
