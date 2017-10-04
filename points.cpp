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
