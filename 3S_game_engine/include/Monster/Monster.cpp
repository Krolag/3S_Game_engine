#include "Monster.h"

Monster::Monster(GameLogic::Proctor* boat, std::vector<GameLogic::Proctor*> zoneVector, WaterMesh* water)
{
	this->boat = boat;
	this->water = water;
	zone = zoneVector;
}

bool Monster::isInSafeZone() //check distance from each tile
{
	int overDistance = 0;
	for (GameLogic::Proctor* t : zone)
	{
		float distance = countDistance(boat->getPosition(), t->getPosition());
		if (distance > SAFE_DISTANCE) overDistance++;
		//std::cout << distance << "\n";
	}

	if (overDistance == zone.size()) return false;
	else return true;

}

float Monster::countDistance(glm::vec3 object_1, glm::vec3 object_2)
{
	return glm::distance(object_1, object_2);
}

void Monster::isPositionChanged(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music, ISound* backgroundSound,GameLogic::Proctor* monster,bool isMusicPlaying)
{
	timeElapsed += boat->getDeltaTime();
	if (timeElapsed > TIME_BETWEEN_POSITIONS_UPDATE) //every 3 seconds update boat position
	{
		oldPosition = currentPosition;
		currentPosition = boat->getPosition();
		float currentDistance = countDistance(currentPosition, oldPosition);
		//printf("%f", currentDistance);

		if (currentDistance > MIN_DISTANCE && heartBeats < 3 && !isDeepWater())
		{
			backgroundSound->setIsPaused(false);
			if (isMusicPlaying) { engine->setAllSoundsPaused(false); }			
			heartBeats = 0;
			timeElapsed = 0;
		}

		if (currentDistance < MIN_DISTANCE || isDeepWater())
		{
			engine->setAllSoundsPaused();
			if(heartBeats < 2) engine->play2D(audio,false);
			heartBeats++;
			timeElapsed = 0;
		}

		if (heartBeats == 3) {
			monster->transform.scale = glm::vec3(0.4, 0.4, 0.4);
			monster->transform.position = currentPosition + glm::vec3(0.0f, -100.0f, 0.0f);
			engine->play2D(music, false);
		}

		if (heartBeats == 4) 
		{
			isGameOver = true;
			backgroundSound->setIsPaused(false);
			if (isMusicPlaying) engine->setAllSoundsPaused(false);
			heartBeats = 0;
		}
	}
}

void Monster::update(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music, ISound* backgroundSound, GameLogic::Proctor* monster, bool isMusicPlaying)
{	
	float colorChange = 0.3 * boat->getParentHierarchy()->getDeltaTime();
	if(heartBeats >= 3 && monster->transform.position.y < -35){
		time += boat->getDeltaTime() * 4;
		monster->transform.position.y += time;
	}
	if (heartBeats > 1 && water->waterColor.z > 0.1) 
	{	
		water->waterColor -= glm::vec3(colorChange,colorChange,colorChange);
		std::cout << water->waterColor.z << "\n";
	}
	else if(water->waterColor.z < 0.9){
		water->waterColor += glm::vec3(colorChange, colorChange, colorChange);
	}

	if (!isInSafeZone())
	{
		isPositionChanged(engine,audio,music, backgroundSound,monster,isMusicPlaying);
	}
	else
	{
		timeElapsed = 0;
	}
}

bool Monster::isDeepWater()
{
	float boat_x = boat->getPosition().x;
	float boat_z = boat->getPosition().z;
	if (boat_x < X_LEFT || boat_x > X_RIGHT || boat_z > Z_UP || boat_z < Z_DOWN) {
		std::cout << "dzban";
		return true;
	}
	return false;
}
