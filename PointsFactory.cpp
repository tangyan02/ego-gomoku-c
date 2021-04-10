#include "stdafx.h"
#include "PointsFactory.h"


points PointsFactory::neighborPoints[30][30];

points PointsFactory::levelProcessorTempPoints;

points PointsFactory::comboProcessorTempPoints;

points PointsFactory::dfsTempPoints[30];

points PointsFactory::comboNeighborPoints[2][50];

points * PointsFactory::createPointNeighborPoints(int level, int extend)
{
	neighborPoints[level][extend].clear();
	return &neighborPoints[level][extend];
}

points * PointsFactory::createComboNeighborPoints(int comboType, int level)
{
	comboNeighborPoints[comboType][level].clear();
	return &comboNeighborPoints[comboType][level];
}

points * PointsFactory::createLevelProcessorTempPoints()
{
	levelProcessorTempPoints.clear();
	return &levelProcessorTempPoints;
}

points* PointsFactory::createcomboProcessorTempPoints()
{
	comboProcessorTempPoints.clear();
	return &comboProcessorTempPoints;
}

points * PointsFactory::createDfsTempPoints(int level)
{
	dfsTempPoints[level].clear();
	return &dfsTempPoints[level];
}
	