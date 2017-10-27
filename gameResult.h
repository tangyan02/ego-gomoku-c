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
	int extend = 0;
	int comboCacheHit = 0;
	int comboCacheTotal = 0;
	gameResult();
	~gameResult();
};

