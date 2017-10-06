#include "stdafx.h"
#include "pointHash.h"

void pointHash::add(int x, int y)
{
	signal[x][y] = true;
}

void pointHash::add(point p)
{
	signal[p.x][p.y] = true;
}

bool pointHash::contains(int x, int y)
{
	return signal[x][y];
}

bool pointHash::contains(point p)
{
	return signal[p.x][p.y];
}

pointHash::pointHash(int size)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			signal[i][j] = false;
		}
	}
}


pointHash::~pointHash()
{

}
