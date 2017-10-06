#pragma once
class pointHash
{
	bool signal[20][20];
public:
	void add(int x, int y);
	void add(point p);
	bool contains(int x, int y);
	bool contains(point p);
	pointHash(int size);
	~pointHash();
};

