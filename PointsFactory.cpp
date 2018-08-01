#include "stdafx.h"
#include "PointsFactory.h"


points PointsFactory::neighborPoints[30];

points PointsFactory::levelProcessorTempPoints;

points PointsFactory::dfsTempPoints[30];

points * PointsFactory::createPointNeighborPoints(int level)
{
	neighborPoints[level].clear();
	return &neighborPoints[level];
}

points * PointsFactory::createLevelProcessorTempPoints()
{
	levelProcessorTempPoints.clear();
	return &levelProcessorTempPoints;
}

points * PointsFactory::createDfsTempPoints(int level)
{
	dfsTempPoints[level].clear();
	return &dfsTempPoints[level];
}
	