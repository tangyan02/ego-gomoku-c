#pragma once
#include<vector>

using namespace std;

class points
{
public:
	vector<point> list;
	int count = 0;

	points();
	~points();
	void add(point p);
	void addMany(points *ps);
	void clear();
	bool contains(int x, int y);
};

