#include "Monster.h"

Monster::Monster(GameLogic::Proctor* boat, std::vector<GameLogic::Proctor*> zoneVector)
{
	this->boat = boat;
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

void Monster::isPositionChanged(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music,GameLogic::Proctor* monster)
{
	timeElapsed += boat->getDeltaTime();
	if (timeElapsed > TIME_BETWEEN_POSITIONS_UPDATE) //every 3 seconds update boat position
	{
		oldPosition = currentPosition;
		currentPosition = boat->getPosition();
		float currentDistance = countDistance(currentPosition, oldPosition);
		//printf("%f", currentDistance);

		if (currentDistance > MIN_DISTANCE)
		{
			engine->setAllSoundsPaused(false);
			heartBeats = 0;
			timeElapsed = 0;
		}

		if (currentDistance < MIN_DISTANCE)
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
		}
	}
}

void Monster::update(ISoundEngine* engine, ISoundSource* audio, ISoundSource* music, GameLogic::Proctor* monster)
{
	if(heartBeats >= 3 && monster->transform.position.y < -35){
		time += boat->getDeltaTime() * 4;
		monster->transform.position.y += time;
	}

	if (!isInSafeZone())
	{
		isPositionChanged(engine,audio,music,monster);
	}
	else
	{
		timeElapsed = 0;
	}
}
