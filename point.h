#pragma once
class point
{
public:
	int x;
	int y;
	point();
	int hash();
	point(int x, int y);
	~point();
};

