#pragma once

#include "points.h"

class PointsFactory
{
public:
	
	static points* createPointNeighborPoints(int level);

	static points* createTempPoints();

	static points neighborPoints[30];

	static points tempPoints;
};

