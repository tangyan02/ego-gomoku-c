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
	list[count++] = p;
}

void points::addMany(points ps)
{
	for (int i = 0; i < ps.count; i++)
	{
		add(ps.list[i]);
	}
}
