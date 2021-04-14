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
	list[count] = p;
	count++;
}

void points::addMany(points *ps)
{
	for (int i = 0; i < ps->count; i++)
	{
		add(list[i]);
	}
}

void points::pop()
{
	count--;
}

void points::clear()
{
	count = 0;
}

bool points::contains(int x, int y)
{
	for (int i = 0; i < count; i++)
	{
		if (list[i].x == x && list[i].y == y) {
			return true;
		}
	}
	return false;
}
