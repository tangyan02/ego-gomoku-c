#include "stdafx.h"
#include "pointHash.h"

extern int boardSize;

void pointHash::add(int x, int y)
{
	signal[x][y] = true;
	count++;
}

void pointHash::add(point p)
{
	signal[p.x][p.y] = true;
	count++;
}

void pointHash::addMany(points ps)
{
	for (int i = 0; i < ps.count; i++)
		add(ps.list[i]);
}

bool pointHash::contains(int x, int y)
{
	return signal[x][y];
}

bool pointHash::contains(point p)
{
	return signal[p.x][p.y];
}

pointHash::pointHash()
{
	reset();
}

pointHash::~pointHash()
{

}

void pointHash::reset()
{
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			signal[i][j] = false;
		}
	}
	count = 0;
}
