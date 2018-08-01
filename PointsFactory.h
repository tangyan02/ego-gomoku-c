#pragma once

#include "points.h"

class PointsFactory
{

private:

	static points neighborPoints[30];

	static points levelProcessorTempPoints;

	static points dfsTempPoints[30];

public:
	
	static points* createPointNeighborPoints(int level);

	static points* createLevelProcessorTempPoints();

	static points* createDfsTempPoints(int level);

	
};

