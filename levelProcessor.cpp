#include "levelProcessor.h"
#include "PointsFactory.h"
#include "analyzer.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "stdafx.h"

int singleScore[3][20][20];

extern int boardSize;

static int scoreList[512];

extern int neighborCount[20][20];

extern int blackLineKey[20][20][4];
extern int whiteLineKey[20][20][4];
extern int patternScore[PATTERN_SIZE];

extern points moveHistory;

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

void qsort(point *list, int score[], int l, int r)
{
	int x = l;
	int y = r;
	int mid = score[(x + y) / 2];
	while (x < y)
	{
		while (score[x] > mid)
			x++;
		while (score[y] < mid)
			y--;
		if (x <= y)
		{
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
	}
	if (x < r)
		qsort(list, score, x, r);
	if (l < y)
		qsort(list, score, l, y);
}

void selectAndSortPoints(points *neighbors, Color color)
{
	if (tryFiveAttack(color, neighbors))
		return;
	if (tryFourDefence(color, neighbors))
		return;
	if (tryThreeDefence(color, neighbors))
		return;

	point *list = neighbors->list;

	for (int i = 0; i < neighbors->count; i++)
	{
		scoreList[i] = getScore(list[i].x, list[i].y, color);
	}
	qsort(list, scoreList, 0, neighbors->count - 1);
}