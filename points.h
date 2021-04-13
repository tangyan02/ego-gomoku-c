#pragma once
#include "point.h"

class points
{
public:
	point list[256];
	int count = 0;

	points();
	~points();
	void add(point p);
	void addMany(points *ps);
	void pop();
	void clear();
	bool contains(int x, int y);
};
