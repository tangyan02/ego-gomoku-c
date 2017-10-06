#include "stdafx.h"
#include "points.h"


points::points()
{
}

points::~points()
{
}

void points::add(point p)
{
	list.push_back(p);
	count++;
}

void points::addMany(points ps)
{
	for (int i = 0; i < ps.count; i++)
	{
		list.push_back(ps.list[i]);
		count++;
	}
}
