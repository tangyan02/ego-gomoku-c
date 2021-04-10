#pragma once

#include "points.h"

class PointsFactory
{

private:

	static points neighborPoints[30][30];

	static points levelProcessorTempPoints;

	static points comboProcessorTempPoints;

	static points dfsTempPoints[30];

	static points comboNeighborPoints[2][50];

public:

	static points* createPointNeighborPoints(int level, int extend);

	static points* createLevelProcessorTempPoints();

	static points* createcomboProcessorTempPoints();

	static points* createDfsTempPoints(int level);

	static points* createComboNeighborPoints(int comboType, int level);
	
};

