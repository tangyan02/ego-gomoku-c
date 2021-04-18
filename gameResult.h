#pragma once
class gameResult
{
public:
	point result;
	int speed = 0;
	int value = 0;
	int node = 0;
	int level = 0;
	int combo = 0;
	int maxExtend = 0;
	int extendNode = 0;
	int cacheHit = 0;
	int cacheCorrect = 0;
	gameResult();
	~gameResult();
};

