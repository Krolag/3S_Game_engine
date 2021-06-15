#pragma once
#include <GameLogic/Proctor.h>

using namespace irrklang;
class Monster
{
public:
	bool isGameOver = false;

	Monster(GameLogic::Proctor* boat, std::vector<GameLogic::Proctor*> islandTiles);
	bool isInSafeZone();
	float countDistance(glm::vec3 object_1, glm::vec3 object_2);
	void isPositionChanged(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music, GameLogic::Proctor* monster);
	void update(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music, GameLogic::Proctor* monster);

private:
	const int SAFE_DISTANCE = 200; // radius of safe zone around island
	const int TIME_BETWEEN_POSITIONS_UPDATE = 3;
	const int MIN_DISTANCE = 18; // minimum distance beetwen current and last boat position 

	float time = 0;
	float timeElapsed = 0;
	glm::vec3 currentPosition;
	glm::vec3 oldPosition;
	int heartBeats = 0;

	std::vector<GameLogic::Proctor*> zone;
	GameLogic::Proctor* boat;

};

