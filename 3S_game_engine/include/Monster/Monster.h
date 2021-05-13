#pragma once
#include <GameLogic/Proctor.h>

class Monster
{
public:
	Monster(GameLogic::Proctor* boat, std::vector<GameLogic::Proctor*> islandTiles);
	bool isInSafeZone();
	float countDistance(glm::vec3 object_1, glm::vec3 object_2);
	bool isPositionChanged();
	void attack();
	void update();

private:
	const int SAFE_DISTANCE = 12; // radius of safe zone around island
	const int TIME_BETWEEN_POSITIONS_UPDATE = 3;
	const int MIN_DISTANCE = 5; // minimum distance beetwen current and last boat position 

	std::vector<GameLogic::Proctor*> tiles;
	GameLogic::Proctor* boat;

	glm::vec3 currentPosition;
	glm::vec3 oldPosition;
	float timeElapsed = 0;
};

