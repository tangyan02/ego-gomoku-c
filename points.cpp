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
	//list[count] = p;
	count++;
}

void points::addMany(points ps)
{
	for (int i = 0; i < ps.count; i++)
	{
		//list[count] = ps.list[i];
	    list.push_back(ps.list[i]);
		count++;
	}
}

void points::clear()
{
	list.clear();
	count = 0;
}
