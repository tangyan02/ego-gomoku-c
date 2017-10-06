#include "stdafx.h"
#include "console.h"
#include "gamemap.h"
#include "io.h"
#include "game.h"
#include "score.h"
#include "analyzeData.h"
#include "analyzer.h"
#include "levelProcessor.h"

void testPlay() {
	Color ** map = readMap();
	point p = search(BLACK, map);
	map[p.x][p.y] = STAR;
	printMap(map);
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
	printMapWithStar(map, ps);
}

void testAnalyze() {
	Color ** map = readMap("analyze.txt");
	initGameMap(map);
	Color color = WHITE;
	initScore(color);
	analyzeData data = getAnalyzeData(color, getNeighbor());

	printf("5 A\n");
	printMapWithStar(map, data.fiveAttack);

	printf("4 A\n");
	printMapWithStar(map, data.fourAttack);

	printf("4 D\n");
	printMapWithStar(map, data.fourDefence);

	printf("3 A\n");
	printMapWithStar(map, data.threeAttack);

	printf("3 D\n");
	printMapWithStar(map, data.threeDenfence);

	printf("2 A\n");
	printMapWithStar(map, data.twoAttack);
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
	testPlay();
	getchar();
	return 0;
}
