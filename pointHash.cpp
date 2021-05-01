#include "stdafx.h"
#include "pointHash.h"
#include "PointsFactory.h"

extern int boardSize;

void pointHash::add(int x, int y)
{
	signal[x][y] = true;
	count++;
}

void pointHash::add(point& p)
{
	signal[p.x][p.y] = true;
	count++;
}

void pointHash::remove(point& p)
{
	signal[p.x][p.y] = false;
	count--;
}

bool pointHash::contains(int x, int y)
{
	return signal[x][y];
}

bool pointHash::contains(point& p)
{
	return signal[p.x][p.y];
}

void pointHash::removeRepeat(points& ps)
{
	//pHash.reset();
	bool repeat = false;
	for (int i = 0; i < ps.count; i++) {
		if (contains(ps.list[i])) {
			repeat = true;
		}
		add(ps.list[i]);
	}
	if (repeat) {
		points* temp = PointsFactory::createTempPoints();
		for (int i = 0; i < ps.count; i++) {
			if (contains(ps.list[i])) {
				temp->add(ps.list[i]);
				remove(ps.list[i]);
			}
		}
		return;
	}
	for (int i = 0; i < ps.count; i++) {
		remove(ps.list[i]);
	}
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
