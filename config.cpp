#include "stdafx.h"
#include "config.h"

int boardSize = 15;

int searchLevel = 10;

int nodeLimit = 99;

int timeOut = 30 * 1000;

int cacheSize = 5000000;

bool debugEnable = false;

Color getOtherColor(Color color)
{
	if (color == 1 || color == 2)
		return 3 - color;
	return NULL;
}
