#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "PointsFactory.h"
#include "analyzer.h"

extern int boardSize;

static int score[512];

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];

int levelScore[10] = { 0, 100000,10000,100,100,50,50,20,2,1 };

extern Color ** map;

int getScore(int x, int y, Color color) {
	int value = 0;
	for (int k = 0; k < 4; k++) {
		if (color == BLACK)
			value += levelScore[blackPattern[x][y][k]];
		if (color == WHITE)
			value += levelScore[whitePattern[x][y][k]];
	}
	return value;
}

void qsort(points *neighbors, int score[], int l, int r) {
	int x = l;
	int y = r;
	int mid = score[(x + y) / 2];
	while (x < y) {
		while (score[x] > mid) x++;
		while (score[y] < mid) y--;
		if (x <= y) {
			int t = score[x];
			score[x] = score[y];
			score[y] = t;
			point p = neighbors->list[x];
			neighbors->list[x] = neighbors->list[y];
			neighbors->list[y] = p;
			x++;
			y--;
		}
	}
	if (x < r) qsort(neighbors, score, x, r);
	if (l < y) qsort(neighbors, score, l, y);
}

void selectAndSortPoints(points *neighbors, Color color) {
	//连长五
	if (checkAndSetInFiveAttack(color, neighbors))
		return;
	//堵冲四
	if (checkAndSetInFourDefence(color, neighbors))
		return;
	//对付活三
	if (checkAndSetInThreeActive(color, neighbors))
		return;

	//排序
	for (int i = 0; i < neighbors->count; i++) {
		score[i] = getScore(neighbors->list[i].x, neighbors->list[i].y, color);
	}
	qsort(neighbors, score, 0, neighbors->count - 1);

}

