#pragma once
class Points
{
public:
	int score = 0;
	Points();
	Points(int _score);
	~Points();

	void setScore(int _score);
	int getScore();
	int addPoints(int pointsToAdd);

private:


};

