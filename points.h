#pragma once
class points
{
public:
	point list[128];
	int count = 0;

	points();
	~points();
	void add(point p);
};

