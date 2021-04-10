#include "stdafx.h"
#include "console.h"
#include "gameMap.h"
#include "io.h"
#include "game.h"
#include "score.h"
#include "levelProcessor.h"
#include "pisqpipe.h"
#include "pattern.h"
#include "patternRecorder.h"
#include "comboProcessor.h"
#include "learn.h"

extern bool debugEnable;

extern bool piskvorkMessageEnable;

extern int boardSize;

extern int searchLevel;

extern int comboTimeOut;

extern int timeOut;

void testPlay() {
	boardSize = 20;
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

void bugTest() {
	boardSize = 20;
	debugEnable = true;
	//searchLevel = 2;
	Color ** map = readMap("input007.txt");
	gameResult result = search(WHITE, map);
	printMapWithStar(map, result.result);
}

void caseTest() {
	piskvorkMessageEnable = true;
	boardSize = 20;
	debugEnable = true;
	comboTimeOut = 3000;
	timeOut = 5000;
	Color **map = readMap("input007.txt");
	gameResult result = search(WHITE, map);
	printMapWithStar(map, result.result);
}

int main()
{
	piskvork();
	//testCombo();
	//bugTest();
	//caseTest();
	//testPlay();
	//testPattern();
	//testPatternRecorder();
	//selfLearn();
	return 0;
}
