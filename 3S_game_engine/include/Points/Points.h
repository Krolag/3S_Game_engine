#pragma once
class Points
{
public:
	
	static Points* getInstance();
	
	~Points();

	void setScore(int _score);
	int getScore();
	int addPoints(int pointsToAdd);

private:
	Points() {};
	int score = 0;
};

