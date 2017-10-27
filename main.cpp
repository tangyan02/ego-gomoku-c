#include "stdafx.h"
#include "console.h"
#include "gamemap.h"
#include "io.h"
#include "game.h"
#include "score.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "levelProcessor.h"
#include "comboProcessor.h"
#include "pisqpipe.h"

extern bool debugEnable;

extern int boardSize;

extern int comboLevel;

void testPlay() {
	boardSize = 20;
	comboLevel = 10;
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

void testCombo() {
	boardSize = 20;
	long long t = getSystemTime();
	Color ** map = readMap();
	//Color ** map = readMap("failCombo.txt");
	Color color = BLACK;
	initGameMap(map);
	initScore(color);
	int level = 17;
	comboResult result = canKill(color, level, getSystemTime(), 9999999999);
	if (result.win) {
		printMapWithStar(map, result.p);
		printf("four win = %d \n", result.fourWin);
	}
	else
		printf("NO\n");
	printf("%lld ms\n", getSystemTime() - t);
}

void testAnalyze() {
	Color ** map = readMap("analyze2.txt");
	initGameMap(map);
	Color color = BLACK;
	initScore(color);
	analyzeData data = getAnalyzeData(color, &getNeighbor());

	printf("5 A\n");
	printMapWithStars(map, data.fiveAttack);

	printf("4 A\n");
	printMapWithStars(map, data.fourAttack);

	printf("4 D\n");
	printMapWithStars(map, data.fourDefence);

	printf("3 A\n");
	printMapWithStars(map, data.threeAttack);

	printf("3 D\n");
	printMapWithStars(map, data.threeDefence);

}

void testNeighbor() {
	Color ** map = readMap("analyze.txt");
	initGameMap(map);
	points ps = getNeighbor();
	for (int i = 0; i < ps.count; i++) {
		point p = ps.list[i];
		map[p.x][p.y] = STAR;
	}
	printMap(map);
}

int main()
{
	piskvork();
	//testCombo();
	testPlay();
	//testAnalyze();
	getchar();
	return 0;
}
