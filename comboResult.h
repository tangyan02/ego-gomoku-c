#pragma once
class comboResult
{
public:
	point p;
	bool timeOut = false;
	bool fourWin = false;
	bool win = false;
	int node = 0;
	void reset();
	comboResult();
	~comboResult();
};

