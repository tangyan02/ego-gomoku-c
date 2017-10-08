#pragma once
class comboResult
{
public:
	point p;
	bool timout = false;
	bool fourWin = false;
	bool win = false;
	void reset();
	comboResult();
	~comboResult();
};

