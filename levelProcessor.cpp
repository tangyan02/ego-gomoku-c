#include "stdafx.h"
#include "levelProcessor.h"
#include "gameMap.h"
#include "patternRecorder.h"
#include "PointsFactory.h"
#include "analyzer.h"

extern int boardSize;

static int scoreList[512];

static int directX[] = { 0, 1, 1, 1 };

static int directY[] = { 1, 1, 0, -1 };

extern int blackPattern[20][20][4];
extern int whitePattern[20][20][4];

extern int blackPatternCountInNull[10];
extern int whitePatternCountInNull[10];

extern int score[10];


extern Color ** map;

int getScore(int x, int y, Color color) {
	int value = 0;
	for (int k = 0; k < 4; k++) {
		if (color == BLACK)
			value += score[blackPattern[x][y][k]];
		if (color == WHITE)
			value += score[whitePattern[x][y][k]];
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
	//������
	if (tryFiveAttack(color, neighbors))
		return;
	//�³���
	if (tryFourDefence(color, neighbors))
		return;
	//�Ը�����
	if (tryThreeDefence(color, neighbors))
		return;

	//����
	for (int i = 0; i < neighbors->count; i++) {
		scoreList[i] = getScore(neighbors->list[i].x, neighbors->list[i].y, color);
	}
	qsort(neighbors, scoreList, 0, neighbors->count - 1);

}

