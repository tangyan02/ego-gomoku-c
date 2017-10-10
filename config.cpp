#include "stdafx.h"
#include "config.h"

int boardSize = 15;

int searchLevel = 20;

int comboLevel = 35;

int nodeLimit = 99;

int timeOut = 20 * 1000;

int comboTimeOut = 10 * 1000;

int cacheSize = 5000000;

bool debugEnable = false;

bool moreAnalyze = false;

Color getOtherColor(Color color)
{
	if (color == 1 || color == 2)
		return 3 - color;
	return NULL;
}
