#include "stdafx.h"
#include "point.h"


point::point()
{
}

int point::hash()
{
	return x * 100 + y;
}

point::point(int x, int y)
{
	this->x = x;
	this->y = y;
}


point::~point()
{
}
