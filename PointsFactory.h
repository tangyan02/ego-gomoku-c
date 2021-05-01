#pragma once

#include "points.h"

class PointsFactory
{

private:

	static points neighborPoints[30][60];

	static points levelProcessorTempPoints;

	static points comboProcessorTempPoints;

	static points dfsTempPoints[100];

	static points comboNeighborPoints[2][100];

	static points tempPoints;

public:

	static points* createPointNeighborPoints(int level, int extend);

	static points* createLevelProcessorTempPoints();

	static points* createcomboProcessorTempPoints();

	static points* createDfsTempPoints(int level);

	static points* createComboNeighborPoints(int comboType, int level);

	static points* createTempPoints();
	
};

