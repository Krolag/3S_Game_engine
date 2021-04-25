#include "Points.h"
Points::Points(int _score)
{
	score = _score;
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
