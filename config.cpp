#include "stdafx.h"
#include "config.h"

int boardSize = 15;

int searchLevel = 8;

Color getOtherColor(Color color)
{
	if (color == 1 || color == 2)
		return 3 - color;
	return NULL;
}
