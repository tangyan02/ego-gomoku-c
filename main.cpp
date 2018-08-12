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
#include "comboProcessor.h"

extern bool debugEnable;

extern int boardSize;

extern int searchLevel;

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
	boardSize = 20;
	debugEnable = true;
	//searchLevel = 2;
	Color ** map = readMap("input008.txt");
	gameResult result = search(BLACK, map);
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
	getchar();
	return 0;
}
