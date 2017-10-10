#include "stdafx.h"
#include "PointSet.h"


void PointSet::add(point p)
{
	if (!hash.contains(p.x, p.y)) {
		ps.add(p);
	}
}

void PointSet::addMany(points ps)
{
	for (int i = 0; i < ps.count; i++)
	{
		add(ps.list[i]);
	}
}

void PointSet::clear()
{
	ps.clear();
	hash.reset();
}

PointSet::PointSet()
{
}


PointSet::~PointSet()
{
}
