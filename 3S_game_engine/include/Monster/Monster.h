#pragma once
#include <GameLogic/Proctor.h>
#include <Water/WaterMesh.h>

using namespace irrklang;
class Monster
{
public:
	bool isGameOver = false;

	Monster(GameLogic::Proctor* boat, std::vector<GameLogic::Proctor*> islandTiles, WaterMesh* water);
	bool isInSafeZone();
	float countDistance(glm::vec3 object_1, glm::vec3 object_2);
	void isPositionChanged(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music, ISound* backgroundSound, GameLogic::Proctor* monster, bool isMusicPlaying);
	void update(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music, ISound* backgroundSound, GameLogic::Proctor* monster, bool isMusicPlaying);

private:
	WaterMesh* water;

	const int X_LEFT=1100;
	const int Z_UP=500;
	const int X_RIGHT=-300;
	const int Z_DOWN=-1100;

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

	bool isDeepWater();

};

