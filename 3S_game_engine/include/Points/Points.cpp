#include "Points.h"

Points* Points::getInstance()
{	
	static Points instance;
	return &instance;
}

Points::~Points()
{
	score = 0;
}

void Points::setScore(int _score)
{
	score = _score;
}

int Points::getScore()
{
	return score;
}

int Points::addPoints(int pointsToAdd)
{
	score += pointsToAdd;
	return score;
}
