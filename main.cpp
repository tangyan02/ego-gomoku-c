#include "stdafx.h"
#include "console.h"
#include "gamemap.h"
#include "io.h"
#include "game.h"
#include "score.h"
#include "levelProcessor.h"
#include "pisqpipe.h"
#include "pattern.h"
#include "patternRecorder.h"

extern bool debugEnable;

extern int boardSize;

extern int comboLevel;

void testPlay() {
	boardSize = 20;
	comboLevel = 30;
	debugEnable = true;
	Color ** map = readMap();
	gameResult result = search(WHITE, map);
	printMapWithStar(map, result.result);
}

void removeStar(Color** map) {
	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			if (map[i][j] == STAR) {
				map[i][j] = NULL;
			}
		}
	}
}

int main()
{
	//piskvork();
	//testPlay();
	//testPattern();
	testPatternRecorder();
	getchar();
	return 0;
}
