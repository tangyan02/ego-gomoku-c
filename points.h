#pragma once
#include<vector>

using namespace std;

class points
{
public:
	//vector<point> list;
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

