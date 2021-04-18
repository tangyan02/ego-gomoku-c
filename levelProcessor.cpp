#include "stdafx.h"
#include "levelProcessor.h"
#include "PointsFactory.h"
#include "analyzer.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "pattern.h"
#include "pointHash.h"

int singleScore[3][20][20];

extern int boardSize;

static int scoreList[512];

extern int neighborCount[20][20];

extern int blackLineKey[20][20][4];
extern int whiteLineKey[20][20][4];
extern int patternScore[PATTERN_SIZE];

extern points moveHistory;

static pointHash pHash;

static int getScore(int x, int y, Color color)
{
	int value = 0;
	for (int k = 0; k < 4; k++)
	{
		if (color == BLACK)
		{
			value += patternScore[blackLineKey[x][y][k]];
			//value += patternScore[whiteLineKey[x][y][k]];
		}
		if (color == WHITE)
		{
			value += patternScore[whiteLineKey[x][y][k]];
			//value += patternScore[blackLineKey[x][y][k]];
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
			x++;
			y--;
		}
	}
	if (x < r)
		qsort(list, score, x, r);
	if (l < y)
		qsort(list, score, l, y);
}

void removeRepeat(points* ps) {
	//pHash.reset();
	bool repeat = false;
	for (int i = 0; i < ps->count; i++) {
		if (pHash.contains(ps->list[i])) {
			repeat = true;
		}
		pHash.add(ps->list[i]);
	}
	if (repeat) {
		points* temp =PointsFactory::createTempPoints();
		for (int i = 0; i < ps->count; i++) {
			if (pHash.contains(ps->list[i])) {
				temp->add(ps->list[i]);
				pHash.remove(ps->list[i]);
			}
		}
		/*printf("ps\n");
		printPoints(*ps);
		printf("temp\n");
		printPoints(*temp);
		ps->clear();
		ps->addMany(temp);*/
		return;
	}
	for (int i = 0; i < ps->count; i++) {
		pHash.remove(ps->list[i]);
	}
}

void sort(points* neighbors, Color color) {
	point* list = neighbors->list;

	for (int i = 0; i < neighbors->count; i++)
	{
		scoreList[i] = getScore(list[i].x, list[i].y, color);
	}
	qsort(list, scoreList, 0, neighbors->count - 1);
}

void selectAndSortPoints(points *neighbors, Color color)
{
	if (tryFiveAttack(color, neighbors)) {
		removeRepeat(neighbors);
		return;
	}
	if (tryFourDefence(color, neighbors)) {
		removeRepeat(neighbors);
		return;
	}
	if (tryActiveFourAttack(color, neighbors)) {
		removeRepeat(neighbors);
		return;
	}
	if (tryDoubleFourAttack(color, neighbors)) {
		removeRepeat(neighbors);
		return;
	}
	if (tryDoubleComboDefence(color, neighbors)) {
		removeRepeat(neighbors);
		//sort(neighbors, color);
		return;
	}
	if (tryThreeDefence(color, neighbors)) {
		removeRepeat(neighbors);
		sort(neighbors, color);
		return;
	}

	sort(neighbors, color);
}