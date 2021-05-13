#include "Monster.h"

Monster::Monster(GameLogic::Proctor* boat, std::vector<GameLogic::Proctor*> islandTiles)
{
	this->boat = boat;
	tiles = islandTiles;
}

bool Monster::isInSafeZone() //check distance from each tile
{
	int overDistance = 0;
	for (GameLogic::Proctor* t : tiles)
	{
		float distance = countDistance(boat->getPosition(), t->getPosition());
		if (distance > SAFE_DISTANCE) overDistance++;
	}

	if (overDistance == tiles.size()) return false;
	else return true;

}

float Monster::countDistance(glm::vec3 object_1, glm::vec3 object_2)
{
	return glm::distance(object_1, object_2);
}

bool Monster::isPositionChanged()
{
	timeElapsed += boat->getDeltaTime();
	if (timeElapsed > TIME_BETWEEN_POSITIONS_UPDATE) //every 3 seconds update boat position
	{
		oldPosition = currentPosition;
		currentPosition = boat->getPosition();
		float currentDistance = countDistance(currentPosition, oldPosition);

		if (currentDistance < MIN_DISTANCE)
		{
			return false;
		}
		timeElapsed = 0;
	}
	return true;
}

void Monster::update()
{
	if (!isInSafeZone())
	{
		if (!isPositionChanged()) { 
			printf("Time to die \n");
		}
	}
	else
	{
		timeElapsed = 0;
	}
}
