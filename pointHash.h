#pragma once
class pointHash
{
	bool signal[20][20];
public:
	int count = 0;
	void add(int x, int y);
	void add(point& p);
	void remove(point& p);
	bool contains(int x, int y);
	bool contains(point& p);
	pointHash();
	~pointHash();
	void reset();
};

