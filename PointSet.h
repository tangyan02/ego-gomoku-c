#pragma once
#include "pointHash.h"

class PointSet
{
public:
	points ps;
	pointHash hash;

	void add(point p);
	void addMany(points ps);
	void clear();
	PointSet();
	~PointSet();
};

