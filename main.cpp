#include "stdafx.h"
#include "console.h"
#include "gamemap.h"
#include "io.h"
#include "game.h"
#include "score.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "levelProcessor.h"
#include "pisqpipe.h"

extern bool debugEnable;

void testPlay() {
	debugEnable = true;
	Color ** map = readMap();
	gameResult result = search(WHITE, map);
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

void testExpand() {
	Color ** map = readMap("expand.txt");
	initGameMap(map);
	Color color = BLACK;
	initScore(color);
	analyzeData data = getAnalyzeData(color, getNeighbor());
	points ps = getExpandPoints(data, getNeighbor());
	printMapWithStars(map, ps);
}

void testAnalyze() {
	Color ** map = readMap("analyze.txt");
	initGameMap(map);
	Color color = WHITE;
	initScore(color);
	analyzeData data = getAnalyzeData(color, getNeighbor());

	printf("5 A\n");
	printMapWithStars(map, data.fiveAttack);

	printf("4 A\n");
	printMapWithStars(map, data.fourAttack);

	printf("4 D\n");
	printMapWithStars(map, data.fourDefence);

	printf("3 A\n");
	printMapWithStars(map, data.threeAttack);

	printf("3 D\n");
	printMapWithStars(map, data.threeDenfence);

	printf("2 A\n");
	printMapWithStars(map, data.twoAttack);
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
	//piskvork();
	testPlay();
	getchar();
	return 0;
}
