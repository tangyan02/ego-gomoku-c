#include "stdafx.h"
#include "PointsFactory.h"


points PointsFactory::neighborPoints[30];

points PointsFactory::tempPoints;

points * PointsFactory::createPointNeighborPoints(int level)
{
	neighborPoints[level].clear();
	return &neighborPoints[level];
}

points * PointsFactory::createTempPoints()
{
	tempPoints.clear();
	return &tempPoints;
}
	