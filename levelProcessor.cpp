#include "stdafx.h"
#include "levelProcessor.h"
#include "PointsFactory.h"
#include "analyzer.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "pattern.h"
#include "pointHash.h"
#include "score.h"

int singleScore[3][20][20];

extern int boardSize;

static int scoreList[512];

extern int neighborCount[20][20];

extern int blackLineKey[20][20][4];
extern int whiteLineKey[20][20][4];
extern int patternScore[PATTERN_SIZE];
extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];
extern int baseScore[10];

extern points moveHistory;

extern int** map;

static pointHash pHash;

static int directX[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static int directY[] = { 1, 1, 0, -1, -1, -1, 0, 1 };

static int getScore(int x, int y, Color color)
{
	int value = 0;
	for (int k = 0; k < 4; k++)
	{
		if (color == BLACK)
		{
			value += patternScore[blackLineKey[x][y][k]];
			value += patternScore[whiteLineKey[x][y][k]] / 2;
		}
		if (color == WHITE)
		{
			value += patternScore[whiteLineKey[x][y][k]];
			value += patternScore[blackLineKey[x][y][k]] / 2;
		}
	}
	return value;
}

void swap(point* list, int* score, int x, int y) {
	int t = score[x];
	score[x] = score[y];
	score[y] = t;
	t = list[x].x;
	list[x].x = list[y].x;
	list[y].x = t;
	t = list[x].y;
	list[x].y = list[y].y;
	list[y].y = t;
}

void qsort(point* list, int* score, int l, int r)
{
	int x = l;
	int y = r;
	int mid = (x + y) / 2;

	if (score[x] > score[mid])
		swap(list, score, x, mid);
	if (score[x] > score[y])
		swap(list, score, x, y);
	if (score[y] < score[mid])
		swap(list, score, y, mid);

	while (x < y)
	{
		while (score[x] > score[mid])
			x++;
		while (score[y] < score[mid])
			y--;
		if (x <= y)
		{
			swap(list, score, x, y);
			x++;
			y--;
		}
	}
	if (x < r) {
		qsort(list, score, x, r);
	}
	if (l < y)
		qsort(list, score, l, y);
}

void sort(points& neighbors, Color color) {
	point* list = neighbors.list;

	for (int i = 0; i < neighbors.count; i++)
	{
		scoreList[i] = getScore(list[i].x, list[i].y, color);
	}

	qsort(list, scoreList, 0, neighbors.count - 1);
}

void filterForNullPattern(points& neighbors, counter& currentCounter) {
	for (int i = 1; i < neighbors.count; i++) {
		if (scoreList[i] == 0) {
			currentCounter.nullPaternCutCount += neighbors.count - i;
			neighbors.count = i;
			break;
		}
	}
}

void selectAndSortPoints(points& neighbors, Color color, counter& currentCounter)
{
	if (tryFiveAttack(color, neighbors)) {
		pHash.removeRepeat(neighbors);
		return;
	}
	if (tryFourDefence(color, neighbors)) {
		pHash.removeRepeat(neighbors);
		return;
	}
	if (tryActiveFourAttack(color, neighbors)) {
		pHash.removeRepeat(neighbors);
		return;
	}
	if (tryDoubleFourAttack(color, neighbors)) {
		pHash.removeRepeat(neighbors);
		return;
	}
	if (tryThreeDefence(color, neighbors)) {
		pHash.removeRepeat(neighbors);
		sort(neighbors, color);
		return;
	}

	sort(neighbors, color);

	filterForNullPattern(neighbors, currentCounter);
}

//===================================���Դ���======================

#include"io.h"
#include "gameMap.h"
#include "console.h"

void testNullPartton() {
	boardSize = 20;
	initPattern();
	map = readMap("levelfilter01.txt");
	initGameMap(map);

	points* ps =  PointsFactory::createTempPoints();
	fillNeighbor(ps);
	counter ct;

	selectAndSortPoints(*ps, BLACK, ct);

	printMapWithStars(map, *ps);

	printPointPatternsInMessage(18,18);
	
}