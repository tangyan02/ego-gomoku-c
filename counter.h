#pragma once
class counter
{
public:
	int extendNodeCount = 0;
	int innerComboNodeCount = 0;
	int innerComboSucNodeCount = 0;
	int cacheHitCount = 0;
	int cacheCorrectCount = 0;
	int nullPaternCutCount = 0;
	int zeroWindowCount = 0;
	int zeroWindowSucCount = 0;

	void reset() {
		extendNodeCount = 0;
		innerComboNodeCount = 0;
		innerComboSucNodeCount = 0;
		cacheHitCount = 0;
		cacheCorrectCount = 0;
		nullPaternCutCount = 0;
		zeroWindowCount = 0;
		zeroWindowSucCount = 0;
	}
};

