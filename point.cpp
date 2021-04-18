#include "stdafx.h"
#include "point.h"


point::point()
{
	x = -1;
	y = -1;
}

point::point(int x, int y)
{
	this->x = x;
	this->y = y;
}

bool point::isNull()
{
	if (x == -1 && y == -1) {
		return true;
	}
	return false;
}


point::~point()
{
}
